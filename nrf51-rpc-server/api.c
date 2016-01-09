#include <string.h>

#ifndef __API_H__
#include "api.h"
#endif

char *api_handler(char *command)
{
    if(strstr(command, "0")) {
        nrf_gpio_pin_clear(RED);
        nrf_gpio_pin_set(GREEN);
        return("0\n");
    }

    if(strstr(command, "1")) {      // Turn on red
        nrf_gpio_pin_clear(GREEN);
        nrf_gpio_pin_set(RED);
        return("0\n");
    }

    if(strstr(command, "2")) {  // Check that red is on
        return(nrf_gpio_pin_read(RED) ? "0\n" : "1\n");
    }
    return("1\n");
}
