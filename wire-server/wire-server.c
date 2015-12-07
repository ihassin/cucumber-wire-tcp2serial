//
//  wire-server.c
//  wire-server
//
//  Created by Itamar on 10/5/14.
//  Copyright (c) 2014 InContext. All rights reserved.
//
#include <stdlib.h>
#include <string.h>

#ifndef __SYSTEM_INCLUDES_H__
#include "system-includes.h"
#endif

#ifndef __WIRE_SERVER_H__
#include "wire-server.h"
#endif

#ifndef __TOKEN_HANDLING_H__
#include "token-handling.h"
#endif

#ifndef __SCENARIO_H__
#include "scenario.h"
#endif

#define kVersion "0.0.0.0"

int api_match_name(wire_context *context, char* name)
{
    int idx = 0;
    APITable *ptr = context->api_table;
    
    while(ptr && *ptr->api_name && ptr->api)
    {
        if(strcmp(name, ptr->api_name) == 0)
        {
            return(idx);
        }
        ptr++; idx++;
    }
    return(-1);
}

int invoke_by_id(int id, struct wire_context *context)
{
    int idx = 0;
    APITable *ptr = context->api_table;
    
    while(ptr && *ptr->api_name && ptr->api)
    {
        if(id == idx)
        {
            if(context->logger)
            {
                char buffer[1128];
                sprintf(buffer, "Calling %s\n", context->api_table[id].api_name);
                (*context->logger) (buffer);
            }
            return((context->api_table[id].api) (context));
        }
        ptr++; idx++;
    }
    return(-1);
}

static void my_logger(char *log)
{
    printf("Logger: %s\n", log);
}

int step_match_callback(wire_context *context)
{
    char *name_to_match = context->request_block.step_match.name_to_match;
    int retVal = api_match_name(context, name_to_match);
    
    if (retVal == -1)
    {
        sprintf(context->outgoing, "[\"success\", []]\n");
    }
    else
    {
        char *var = getGherkinVar(name_to_match);
        static char buff[1024];
        if(var)
        {
            sprintf(buff, "{\"val\":\"%s\", \"pos\":%d}", var, getGherkinVarPosition(name_to_match, var));
        }
        sprintf(context->outgoing, "[\"success\",[{\"id\":\"%d\", \"args\":[%s]}]]\n", retVal, buff);
    }
    return(0);
}

int step_snippet_callback(wire_context *context)
{
    int retVal = api_match_name(context, context->request_block.step_match.name_to_match);
    
    if (retVal == -1)
    {
        sprintf(context->outgoing, "[\"success\", []]\n");
    }
    else
    {
        sprintf(context->outgoing, "[\"success\",[\"int %s(void) { return %d; } \"]]\n", context->request_block.step_match.name_to_match, retVal);
    }
    return(0);
}

int invoke_callback(wire_context *context)
{
    if(context->logger)
    {
        char buffer[100];
        sprintf(buffer, "Invoking %d\n", context->request_block.step_invoke.id);
        (*context->logger) (buffer);
    }
    
    return(invoke_by_id(context->request_block.step_invoke.id, context));
}

int wire_server(wire_context *context)
{
	int retVal = 0;

	if (context->port <= 0)
	{
		return(1);
	}

	if(context->logger)
	{
		(*context->logger) ("wire_server: Started\n");
	}

	retVal = (*context->listener) (context);

	if(context->logger)
	{
		(*context->logger) ("wire_server: Shut down\n");
	}
	return(retVal);
}

int run_server(char *serialPortName, int port, int single_scenario, int logging)
{
    extern APITable api_table[];
    int retVal;
    wire_context *context;
    
    context = malloc(sizeof(wire_context));
    memset(context, 0, sizeof(wire_context));
    
    context->port                       = port;
    context->single_scenario            = single_scenario;
    context->logger                     = logging ? my_logger : 0;

    context->begin_callback             = begin_callback;
    context->end_callback               = end_callback;
    context->listener                   = (wire_listener) wire_listener_default;
    context->step_match_callback        = step_match_callback;
    context->invoke_callback            = invoke_callback;
    context->step_snippet_callback      = step_snippet_callback;
    context->api_table                  = api_table;
    strcpy(context->serialPortName, serialPortName);

    retVal = wire_server(context);
    return(retVal);
}

char *wire_version()
{
	return(kVersion);
}

