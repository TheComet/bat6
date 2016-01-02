#include "models/COMPort.h"

// ----------------------------------------------------------------------------
// C code for handling port open/close on linux
// ----------------------------------------------------------------------------

#ifdef Q_OS_UNIX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/stat.h>

static int open_port(const char* fd_name)
{
    int fd = open(fd_name, O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd == -1)
    {
        const char* fmt = "open_port: Unable to open %s - ";
        int len = snprintf(NULL, 0, fmt, fd_name);
        char* msg = (char*)malloc(len * sizeof(char) + 1);
        sprintf(msg, fmt, fd_name);
        perror(msg);
        free(msg);
        return -1;
    }

    fcntl(fd, F_SETFL, 0);

    {
        struct termios options;
        tcgetattr(fd, &options);

        /* baud rate */
        cfsetispeed(&options, B115200);
        cfsetospeed(&options, B115200);

        /* raw mode */
        cfmakeraw(&options);

        /* 8 data bits */
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;

        /* even parity */
        options.c_cflag |= PARENB;
        options.c_cflag &= ~(PARODD | CSTOPB);

        tcsetattr(fd, TCSANOW, &options);
    }

    /* set descriptor to non-blocking for read() */
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);

    return fd;
}

static void close_port(int fd)
{
    close(fd);
}

static int check_fd_fine(int fd)
{
    struct stat stat_;

    if(fcntl(fd, F_GETFL) != -1 && fstat(fd, &stat_) != -1 && stat_.st_nlink >= 1)
        return 0;

    return -1;
}

// ----------------------------------------------------------------------------
#else
#   error Platform not supported
#endif

// ----------------------------------------------------------------------------
// Polls read() and emits bytes when they arrive
// ----------------------------------------------------------------------------
ListenerThread::ListenerThread(QObject* parent, int fd) :
    QThread(parent),
    fd(fd)
{
}

// ----------------------------------------------------------------------------
void ListenerThread::run()
{
    char buf;
    isRunning = true;

    while(isRunning)
    {
        // Run this every time before reading to make sure USB cable hasn't been
        // disconnected
        if(check_fd_fine(this->fd) != 0)
        {
            // Causes the main thread to close the file handle and attempt to
            // re-open it using the same name
            emit fileDescriptorInvalidated();
            break;
        }

        // File handle is valid, read one byte and emit it (if the byte is
        // available of course). read() is non-blocking and could return
        // nothing.
        if(read(this->fd, &buf, 1) != -1)
            emit byteReceived(buf);

        // Prevent 100% CPU
        this->msleep(5);
    }

    isRunning = false;
}

// ----------------------------------------------------------------------------
void ListenerThread::exit()
{
    // Set this to false so the read() loop terminates
    isRunning = false;
    QThread::exit();
}

// ----------------------------------------------------------------------------
// COM port class
// ----------------------------------------------------------------------------
COMPort::COMPort() :
    listenerThread(nullptr),
    fd(0)
{
}

// ----------------------------------------------------------------------------
COMPort::~COMPort()
{
    this->close();
}

// ----------------------------------------------------------------------------
bool COMPort::open(const QString& portName)
{
    this->setPortName(portName);
    if(!this->openFileDescriptor())
        return false;
    this->startListenerThread();

    return true;
}

// ----------------------------------------------------------------------------
void COMPort::close()
{
    this->stopListenerThread();
    this->closeFileDescriptor();

    this->portName = "";
}

// ----------------------------------------------------------------------------
bool COMPort::send(const char* data)
{
    if(this->fd <= 0 || write(fd, data, 1) == -1)
    {
        // write failed, try to re-open connection
        if(!this->attemptReOpen())
            return false;
        // maybe it's open again?
        if(write(fd, data, 1) == -1)
            return false;
    }

    return true;
}

// ----------------------------------------------------------------------------
void COMPort::startListenerThread()
{
    if(this->listenerThread)
        this->stopListenerThread();

    this->listenerThread = new ListenerThread(this, this->fd);
    this->connect(listenerThread, SIGNAL(byteReceived(char)),
                  this,           SLOT(onByteReceived(char)));
    this->connect(listenerThread, SIGNAL(fileDescriptorUnexpectedlyClosed()),
                  this,           SLOT(onFileDescriptorUnexpectedlyClosed()));
    this->listenerThread->start();
}

// ----------------------------------------------------------------------------
void COMPort::stopListenerThread()
{
    if(!this->listenerThread)
        return;

    this->listenerThread->exit();
    this->listenerThread->wait();

    delete this->listenerThread;
    this->listenerThread = nullptr;
}

// ----------------------------------------------------------------------------
void COMPort::setPortName(const QString& portName)
{
    this->portName = portName;
}

// ----------------------------------------------------------------------------
bool COMPort::openFileDescriptor()
{
    // This function opens the file set with setPortName()

    if(this->fd > 0)
        this->closeFileDescriptor();

    if((this->fd = open_port(this->portName.toLatin1().data())) == -1)
        return false;

    return true;
}

// ----------------------------------------------------------------------------
void COMPort::closeFileDescriptor()
{
    if(this->fd > 0)
        close_port(this->fd);
    this->fd = 0;
}

// ----------------------------------------------------------------------------
bool COMPort::attemptReOpen()
{
    this->closeFileDescriptor();
    if(this->openFileDescriptor())
    {
        this->startListenerThread();
        return true;
    }

    return false;
}

// ----------------------------------------------------------------------------
void COMPort::onByteReceived(char data)
{
    char buf[2];
    *buf = data;
    buf[1] = '\0';
    emit dataReceived(buf);
}

// ----------------------------------------------------------------------------
void COMPort::onFileDescriptorUnexpectedlyClosed()
{
    this->closeFileDescriptor();
}
