#include <string.h>
#include <stdlib.h>

#ifndef __API_H__
#include "api.h"
#endif

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

#if 0
                // Do some argument finding testing
                char arg[20] = "";
                bool res = find_argument("", 0, arg, 20);
                ESP_LOGI(TAG, "%d find_argument %x - >>%s<<\r\n",__LINE__, (int) res, arg);
                strcpy(arg, "###");
                res = find_argument("EXEC 0 []", 0, arg, 20);
                ESP_LOGI(TAG, "%d find_argument %x - >>%s<<\r\n",__LINE__, (int) res, arg);
                strcpy(arg, "###");
                res = find_argument("EXEC 0 []", 1, arg, 20);
                ESP_LOGI(TAG, "%d find_argument %x - >>%s<<\r\n",__LINE__, (int) res, arg);
                strcpy(arg, "###");
                res = find_argument("EXEC 0 [\"hello\"]", 0, arg, 20);
                ESP_LOGI(TAG, "%d find_argument %x - >>%s<<\r\n",__LINE__, (int) res, arg);
                strcpy(arg, "###");
                res = find_argument("EXEC 0 [\"hello\"]", 2, arg, 20);
                ESP_LOGI(TAG, "%d find_argument %x - >>%s<<\r\n",__LINE__, (int) res, arg);
                strcpy(arg, "###");
                res = find_argument("EXEC 0 [\"hello\", \"world\", \"no2\", \"2\"]", 2, arg, 20);
                ESP_LOGI(TAG, "%d find_argument %x - >>%s<<\r\n",__LINE__, (int) res, arg);
                strcpy(arg, "###");
                res = find_argument("EXEC 0 [\"hello]", 0, arg, 20);
                ESP_LOGI(TAG, "%d find_argument %x - >>%s<<\r\n",__LINE__, (int) res, arg);
                strcpy(arg, "###");
                res = find_argument("EXEC 0 [\"lo\"", 0, arg, 20);
                ESP_LOGI(TAG, "%d find_argument %x - >>%s<<\r\n",__LINE__, (int) res, arg);
                strcpy(arg, "###");
                res = find_argument("EXEC 0 [\"", 0, arg, 20);
                ESP_LOGI(TAG, "%d find_argument %x - >>%s<<\r\n",__LINE__, (int) res, arg);
                strcpy(arg, "###");
                res = find_argument("EXEC 0 [\"a nice long argument\"]", 0, arg, 10);
                ESP_LOGI(TAG, "%d find_argument %x - >>%s<<\r\n",__LINE__, (int) res, arg);
#endif


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
            case 0:
            {
                // We expect a single argument giving which LED to change
                char pinArg[20];
                if (find_argument(command, 0, pinArg, 20))
                {
                    if (strcmp(pinArg, "red") == 0)
                    {
                        gpio_reset_pin(RED);
                        // Have to set it to I/O mode so we can call gpio_get_level later
                        // in this slightly contrived example
                        gpio_set_direction(RED, GPIO_MODE_INPUT_OUTPUT);
                        gpio_reset_pin(GREEN);
                        gpio_set_direction(GREEN, GPIO_MODE_OUTPUT);
                        return 0;
                    }
                    // else it was an LED we don't recognise
                }
                // else we didn't get an argument, that's a fail
                return 1;
            }
            break;
            case 1:
            {
                gpio_set_level(GREEN, 0);
                gpio_set_level(RED, 1);
                return 0;
            }
            break;
            case 2:
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
                                return((gpio_get_level(RED) == level) ? 0 : 1);
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
