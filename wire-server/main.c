#include <stdlib.h>

#ifndef __WIRE_SERVER_H__
#include "wire-server.h"
#endif

int main(int argc, char **argv)
{
    return(run_server(argv[1], atoi(argv[2]), 1, 1));  // with logging
    //return(run_server(argv[1], atoi(argv[2]), 1, 0));   // without logging
}
