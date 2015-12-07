#ifndef __WIRE_LISTENER_H__
#define __WIRE_LISTENER_H__

#ifndef __TOKEN_HANDLING_H__
#include "token-handling.h"
#endif

#ifndef __CONTEXT_H__
#include "context.h"
#endif

typedef struct
{
    char *packet;
} ProtocolPacket;

typedef ssize_t (*net_reader) (int socket, char *buffer);

int handleRequest(char *buffer, struct wire_context *context);
char *handle_callback(wire_feature_callback callback, struct wire_context *context);
ssize_t getRequest(net_reader reader, int socket, char *buffer, ssize_t len);

ssize_t getNetworkByte(int socket, char *buffer);
void cleanup(int socket1, int socket2);
int acceptConnection(int sockfd);
int makeSocket(int port);

#define LOG(m) { if (context->logger && m) (*context->logger) (m); }

#endif
