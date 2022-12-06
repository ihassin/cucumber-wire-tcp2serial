#include <string.h>

#ifndef __API_H__
#include "api.h"
#endif

#define LED_ON 1
#define LED_OFF !LED_ON

static struct gpio_dt_spec red_led = GPIO_DT_SPEC_GET(RED, gpios);
static struct gpio_dt_spec green_led = GPIO_DT_SPEC_GET(GREEN, gpios);

void init_leds(void)
{
    gpio_pin_configure_dt(&red_led, GPIO_INPUT | GPIO_OUTPUT_HIGH);
    gpio_pin_configure_dt(&green_led, GPIO_OUTPUT_HIGH);
}

char *api_handler(char *command)
{
    if(strstr(command, "0")) {
        gpio_pin_set_dt(&red_led, LED_OFF);
        gpio_pin_set_dt(&green_led, LED_ON);
        return("0\n");
    }

    if(strstr(command, "1")) {      // Turn on red
        gpio_pin_set_dt(&red_led, LED_ON);
        gpio_pin_set_dt(&green_led, LED_OFF);
        return("0\n");
    }

    if(strstr(command, "2")) {  // Check that red is on
        return (gpio_pin_get_dt(&red_led) ? "0\n" : "1\n");
    }
    return("1\n");
}
