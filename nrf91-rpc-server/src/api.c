#include <string.h>
#include <stdlib.h>

#ifndef __API_H__
#include "api.h"
#endif

// Include the step constants
#include "steps.h"

#define LED_ON 1
#define LED_OFF !LED_ON

static struct gpio_dt_spec red_led = GPIO_DT_SPEC_GET(RED, gpios);
static struct gpio_dt_spec green_led = GPIO_DT_SPEC_GET(GREEN, gpios);

// Look through a command string to find an argument
// Commands are of the form 'EXEC <ID> ["<ARG1>", "<ARG2>"]'
// Parameters:
//      aCommand - command string to search through
//      aArgumentNumber - which argument to get, starting from 0
//      aDest - buffer into which the argument is copied (untouched if none found)
//      aDestLen - size of the buffer in aDest
// Returns true if the argument was found, else false
bool find_argument(const char* aCommand, int aArgumentNumber, char* aDest, int aDestLen)
{
    // Find the start of the arguments
    char* args = strstr(aCommand, "[");
    if (args)
    {
        char* openquotes;
        char* closequotes = args;
        do
        {
            // Look for "
            openquotes = strstr(closequotes+1, "\"");
            if (openquotes)
            {
                // Then find the next "
                closequotes = strstr(openquotes+1, "\"");
            }
        }
        while (openquotes && (aArgumentNumber-- > 0));

        // We've either stopped finding arguments, or gotten to the one we want
        if (openquotes && closequotes)
        {
            // We've got an argument
            int argLen = closequotes-openquotes-1;
            // Make sure there's room for it, or truncate it (allowing for a NUL-terminator)
            argLen = (argLen < aDestLen-1 ? argLen : aDestLen-1);
            strncpy(aDest, openquotes+1, argLen);
            // Ensure the destination string is NUL-terminated, as strncpy doesn't!
            aDest[argLen] = '\0';
            return true;
        }
        // else we didn't find the requested argument, fall through to return false
    }
    return false;
}

void init_leds(void)
{
    gpio_pin_configure_dt(&red_led, GPIO_INPUT | GPIO_OUTPUT_HIGH);
    gpio_pin_configure_dt(&green_led, GPIO_OUTPUT_HIGH);
}

int api_handler(char *command)
{
    // Look for the "EXEC " leading text
    const char kCmdPrefix[6] = "EXEC ";
    char* prefix = strstr(command, kCmdPrefix);
    if (prefix)
    {
        // Parse the command.
        // Initially that'll just mean turning the first arg into a number
        char* parameters = NULL;
        long cmd = strtol(command+strlen(kCmdPrefix), &parameters, 0);
        if (parameters != command)
        {
            // strtol found a number at the start, so cmd is valid
            switch (cmd)
            {
            case I_HAVE_A_PARAM_LED:
            {
                // We expect a single argument giving which LED to change
                char pinArg[20];
                if (find_argument(command, 0, pinArg, 20))
                {
                    if (strcmp(pinArg, "red") == 0)
                    {
                        gpio_pin_set_dt(&red_led, LED_OFF);
                        gpio_pin_set_dt(&green_led, LED_ON);
                        return 0;
                    }
                    // else it was an LED we don't recognise
                }
                // else we didn't get an argument, that's a fail
                return 1;
            }
            break;
            case I_TURN_IT_ON:
            {
                gpio_pin_set_dt(&red_led, LED_ON);
                gpio_pin_set_dt(&green_led, LED_OFF);
                return 0;
            }
            break;
            case THE_PARAM_LED_IS_PARAM:
            {
                // We expect two arguments giving which LED to check, and its level
                char pinArg[20];
                char levelArg[20];
                if (find_argument(command, 0, pinArg, 20))
                {
                    // Got the first argument, find the second
                    if (find_argument(command, 1, levelArg, 20))
                    {
                        if (strcmp(pinArg, "red") == 0)
                        {
                            // We only support the "red" LED at present
                            int level = -1;
                            if (strcmp(levelArg, "lit up") == 0)
                            {
                                level = 1;
                            }
                            else if (strcmp(levelArg, "turned off") == 0)
                            {
                                level = 0;
                            }

                            if (level >= 0)
                            {
                                return ((gpio_pin_get_dt(&red_led) == level) ? 0 : 1);
                            }
                        }
                    }
                }
                return 1;
            }
            break;
            default:
                return 1;
            };
        }
    }

    // Default to failing
    return 1;
}
