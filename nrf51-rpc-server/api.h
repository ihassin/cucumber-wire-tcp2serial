#ifndef __API_H__
#define __API_H__

//#include "nrf_gpio.h"
#include "boards.h"

#define GREEN   LED_1
#define RED     LED_2

void init_leds();
char *api_handler(char *command);

#endif
