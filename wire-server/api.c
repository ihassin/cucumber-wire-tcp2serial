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

    (*context->logger) ("i_have_a_red_led called\n");
    int retVal = serial_write(context->serialPortHandle, "EXEC 0\n");
    serial_read(context->serialPortHandle, buff, sizeof(buff));
    printf("i_have_a_red_led: <%s>\n", buff);
	return(retVal);
}

static int i_turn_it_on(struct wire_context *context)
{
    char buff[20];

    (*context->logger) ("i_turn_it_on called\n");
    int retVal = serial_write(context->serialPortHandle, "EXEC 1\n");
    serial_read(context->serialPortHandle, buff, sizeof(buff));
    printf("i_turn_it_on: <%s>\n", buff);
    return(retVal);
}

// <["invoke",{"id":"3","args":["scenario"]}]>
static int it_is_lit_up(struct wire_context *context)
{
    char buff[20];

    (*context->logger) ("it_is_lit_up called\n");
    int retVal = serial_write(context->serialPortHandle, "EXEC 2\n");
    serial_read(context->serialPortHandle, buff, sizeof(buff));
    printf("it_is_lit_up: <%s>\n", buff);
    return(retVal);
}
