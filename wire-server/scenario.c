#include <stdio.h>

#ifndef __SCENARIO_H__
#include "scenario.h"
#endif

#ifndef __SERIAL_H__
#include "serial.h"
#endif

int begin_callback(struct wire_context *context)
{
	printf("begin_callback(): context->serialPortName == %s\n", context->serialPortName);
    context->serialPortHandle = serial_open(context->serialPortName);
    return(context->serialPortHandle > 0 ? 0 : 1);
}

int end_callback(struct wire_context *context)
{
    serial_close(context->serialPortHandle);
    return(0);
}

