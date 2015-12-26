#include "models/COMPort.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

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
        cfsetispeed(&options, B1152000);
        cfsetospeed(&options, B1152000);

        /* enable receiver and set local mode */
        options.c_cflag |= (CLOCAL | CREAD);

        /* 8 data bits */
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;

        /* even parity */
        options.c_cflag |= PARENB;
        options.c_cflag &= ~(PARODD | CSTOPB | CSIZE);

        tcsetattr(fd, TCSANOW, &options);
    }

    return fd;
}

static void close_port(int fd)
{
    close(fd);
}

ListenerThread::ListenerThread(QObject* parent, int fd) :
    QThread(parent),
    fd(fd)
{
}

void ListenerThread::run()
{
    char buf;
    isRunning = true;
    while(isRunning)
    {
        read(this->fd, &buf, 1);
        emit byteReceived(buf);
    }
}

void ListenerThread::exit()
{
    isRunning = false;
    QThread::exit();
}

COMPort::COMPort() :
    listenerThread(nullptr),
    fd(0)
{
}

COMPort::~COMPort()
{
    this->close();
}

bool COMPort::open(const QString& comName)
{
    if(this->fd > 0)
        this->close();

    if((this->fd = open_port(comName.toLatin1().data())) == -1)
        return false;

    this->listenerThread = new ListenerThread(this, this->fd);
    this->connect(listenerThread, SIGNAL(dataReceived(char)), this, SLOT(onDataReceived(char)));
    this->listenerThread->start();

    return true;
}

void COMPort::close()
{
    if(this->listenerThread)
        this->listenerThread->exit();
    delete this->listenerThread;
    this->listenerThread = nullptr;

    if(this->fd > 0)
        close_port(this->fd);
    this->fd = 0;
}

bool COMPort::send(const char* data) const
{
    if(this->fd <= 0)
        return false;

    if(write(fd, data, strlen(data)))
        return false;
    return true;
}

void COMPort::onByteReceived(char data)
{
    char buf[2];
    *buf = data;
    buf[1] = '\0';
    emit dataReceived(buf);
}
