#ifndef __API_H__
#define __API_H__

#include "driver/gpio.h"

// Define GPIO pins for the LEDs
#define GREEN   14
#define RED     15

void init_leds();
int api_handler(char *command);

#endif
