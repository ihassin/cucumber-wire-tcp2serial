#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdio.h>

#ifndef __SERIAL_H__
#include "serial.h"
#endif

int serial_read(int serialPortHandle, char *command, int maxBytes)
{
    int retVal;
    int idx = 0;

    command[0] = 0;
    while(idx < maxBytes)
    {
        char chr = 0;
        int len = read(serialPortHandle, &chr, 1);
        if(len < 0)
        {
            break;
        }
        if(len > 0)
        {
            if(chr == '\n')
            {
                break;
            }
            *command++ = chr;
        }
    }
    *command = 0;
    return(0);
}

int serial_write(int serialPortHandle, char *command)
{
    int len = strlen(command);
    int retVal = write(serialPortHandle, command, len);
    return(len == retVal ? 0 : retVal);
}

int serial_open(char *serialPortName)
{
    struct termios theTermios;

    int fd = open(serialPortName, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    if(fd == -1)
    {
        return(0);
    }

    memset(&theTermios, 0, sizeof(struct termios));
    cfmakeraw(&theTermios);
    cfsetspeed(&theTermios, B38400);

    theTermios.c_cflag = CREAD | CLOCAL | IGNPAR | CS8 | IXON;
    theTermios.c_cflag &= ~PARENB;
    theTermios.c_cflag &= ~CSTOPB;

    theTermios.c_cc[VMIN] = 0;
    theTermios.c_cc[VTIME] = 100;

    ioctl(fd, TIOCSETA, &theTermios);
    return(fd);
}

void serial_close(int serialPortHandle)
{
    close(serialPortHandle);
}
