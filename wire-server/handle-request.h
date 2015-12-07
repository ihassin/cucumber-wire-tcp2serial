#ifndef __HANDLE_REQUEST_H__
#define __HANDLE_REQUEST_H__

#ifndef __SYSTEM_INCLUDES_H__
#include "system-includes.h"
#endif

#ifndef __WIRE_SERVER_H__
#include "wire-server.h"
#endif

#ifndef __WIRE_LISTENER_H__
#include "wire-listener.h"
#endif

int handleRequest(char *buffer, wire_context *context);

#endif

