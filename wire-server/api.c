#include <string.h>
#include <stdio.h>

#ifndef __API_H__
#include "api.h"
#endif

#ifndef __WIRE_SERVER_H__
#include "wire-server.h"
#endif

#ifndef __TOKEN_HANDLING_H__
#include "token-handling.h"
#endif

#ifndef __SERIAL_H__
#include "serial.h"
#endif

static int i_have_a_red_led(struct wire_context *context);
static int i_turn_it_on(struct wire_context *context);
static int it_is_lit_up(struct wire_context *context);

APITable api_table[] = {
      { "I have a red led",     i_have_a_red_led    }
    , { "I turn it on",         i_turn_it_on        }
    , { "it's lit up",          it_is_lit_up        }
    , 0
};

static int i_have_a_red_led(struct wire_context *context)
{
    char buff[20];

    serial_write(context->serialPortHandle, "EXEC 0\r");
    serial_read(context->serialPortHandle, buff, sizeof(buff));
    printf("api.c: buff: <<%s>>\n", buff);
	return(*buff == '0' ? 0 : 1);
}

static int i_turn_it_on(struct wire_context *context)
{
    char buff[20];

    serial_write(context->serialPortHandle, "EXEC 1\r");
    serial_read(context->serialPortHandle, buff, sizeof(buff));
    return(*buff == '0' ? 0 : 1);
}

static int it_is_lit_up(struct wire_context *context)
{
    char buff[20];

    serial_write(context->serialPortHandle, "EXEC 2\r");
    serial_read(context->serialPortHandle, buff, sizeof(buff));
    return(*buff == '0' ? 0 : 1);
}
