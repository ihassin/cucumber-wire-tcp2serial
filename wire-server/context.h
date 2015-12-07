#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#ifndef __API_H__
#include "api.h"
#endif

struct wire_context;

typedef void (*wire_logger) (char *log);
typedef int (*wire_listener) (struct wire_context *context);
typedef char *(*wire_packet_injector) (struct wire_context *context);
typedef int (*wire_feature_callback) (struct wire_context *context);
typedef int (*wire_step_match_callback) (struct wire_context *context);
typedef int (*wire_step_snippet_callback) (struct wire_context *context);
typedef int (*wire_invoke_callback) (struct wire_context *context);

typedef struct step_match {
    char name_to_match[1024];
} StepMatch;

typedef struct step_invoke {
    int id;
} StepInvoke;

typedef struct wire_context
{
    int                             port;
    int                             single_scenario;
    APITable                        *api_table;
    wire_logger                     logger;
    wire_feature_callback           begin_callback;
    wire_feature_callback           end_callback;
    wire_listener                   listener;
    wire_packet_injector            packet_injector;
    wire_packet_injector            packet_dejector;
    wire_step_match_callback        step_match_callback;
    wire_step_snippet_callback      step_snippet_callback;
    wire_invoke_callback            invoke_callback;
    char                            incoming[1024];
    char                            outgoing[1024];
    int                             serialPortHandle;
    char                            serialPortName[512];
    union
    {
        StepMatch   step_match;
        StepInvoke  step_invoke;
    } request_block;
} wire_context;

#endif
