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
	    //printf("serial_read(): len: %d\n", len);
	    //printf("errno: %d\n", errno);

	    // appears to work with or without the break commented out.
	    // it was uncommented to begin with so I'll leave it like this.
            break;
        }
        if(len > 0)
        {
	    printf("serial_read(): chr: %c = %x\n", chr, chr);
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
    printf("serial_write(): retVal: %d\n", retVal);
    printf("serial_write(): command: %s\n", command);
    return(len == retVal ? 0 : retVal);
}

int serial_open(char *serialPortName)
{
    int fd = open (serialPortName, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        printf("error %d opening %s: %s\n", errno, serialPortName, strerror (errno));
        return fd;
    }

    struct termios tty;
    if (tcgetattr (fd, &tty) != 0)
    {
        printf("error %d from tcgetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    speed_t speed = B115200;
    cfsetospeed (&tty, speed);
    cfsetispeed (&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
                                    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    //tty.c_cc[VMIN]  = 0;            // read doesn't block
    //tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    int parity = 0;
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
    {
        printf("error %d from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }
    return fd;
}

void serial_close(int serialPortHandle)
{
    close(serialPortHandle);
}
