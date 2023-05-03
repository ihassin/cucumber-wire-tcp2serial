#ifndef __API_H__
#define __API_H__

#include <drivers/gpio.h>

#define RED		DT_ALIAS(led0)
#define GREEN		DT_ALIAS(led1)

void init_leds();
int api_handler(char *command);

#endif
