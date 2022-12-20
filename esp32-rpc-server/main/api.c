#include <string.h>

#ifndef __API_H__
#include "api.h"
#endif

char *api_handler(char *command)
{
    if(strstr(command, "0")) {
        gpio_reset_pin(RED);
        // Have to set it to I/O mode so we can call gpio_get_level later
        // in this slightly contrived example
        gpio_set_direction(RED, GPIO_MODE_INPUT_OUTPUT);
        gpio_reset_pin(GREEN);
        gpio_set_direction(GREEN, GPIO_MODE_OUTPUT);
        return("0\n");
    }

    if(strstr(command, "1")) {      // Turn on red
        gpio_set_level(GREEN, 0);
        gpio_set_level(RED, 1);
        return("0\n");
    }

    if(strstr(command, "2")) {  // Check that red is on
        return(gpio_get_level(RED) ? "0\n" : "1\n");
    }
    return("1\n");
}
