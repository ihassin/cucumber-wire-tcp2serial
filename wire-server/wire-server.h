//
//  wire-server.h
//  wire-server
//
//  Created by Itamar on 10/5/14.
//  Copyright (c) 2014 InContext. All rights reserved.
//
#ifndef __WIRE_SERVER_H__
#define __WIRE_SERVER_H__

#ifndef __API_H__
#include "api.h"
#endif

#ifndef __CONTEXT_H__
#include "context.h"
#endif

int api_match_name(struct wire_context *context, char* name);
int invoke_by_id(int id, struct wire_context *context);
int wire_server(struct wire_context *context);
int wire_listener_default(struct wire_context *context);
char *wire_version(void);
int run_server(char *serialPortName, int port, int single_scenario, int logging);

#endif
