#ifndef __API_H__
#define __API_H__

struct wire_context;

typedef int (*api_function) (struct wire_context *context);

typedef struct function_table
{
	char api_name[1024];
	api_function api;
} APITable;

#endif
