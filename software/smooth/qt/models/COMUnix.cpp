#include "models/COMUnix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

int
open_port(const char* fd_name)
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
        cfsetispeed(&options, B38400);
        cfsetospeed(&options, B38400);

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

bool COMUnix::open(const QString& comName)
{

}

void COMUnix::close()
{

}
