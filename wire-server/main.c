#include <stdlib.h>

#ifndef __WIRE_SERVER_H__
#include "wire-server.h"
#endif

#include <stdio.h>

int main(int argc, char **argv)
{
	printf("sizeof(int) = %ld\n", sizeof(int));
    //return(run_server(argv[1], atoi(argv[2]), 1, 1));  // with logging
    return(run_server(argv[1], atoi(argv[2]), 1, 0));   // without logging
}
