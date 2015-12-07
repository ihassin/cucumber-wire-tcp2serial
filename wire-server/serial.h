#ifndef __UART_H__
#define __UART_H__

int serial_open(char *serialPortName);
void serial_close(int serialPortHandle);
int serial_write(int serialPortHandle, char *command);
int serial_read(int serialPortHandle, char *command, int maxBytes);

#endif
