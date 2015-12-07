#ifndef __SYSTEM_INCLUDES_H__
#include "system-includes.h"
#endif

#ifndef __WIRE_SERVER_H__
#include "wire-server.h"
#endif

#ifndef __WIRE_LISTENER_H__
#include "wire-listener.h"
#endif

int wire_listener_default(wire_context *context)
{
    char buffer[1024];
    int newsockfd;
    size_t retVal;
    int sockfd;
    size_t sent;

    sockfd = makeSocket(context->port);
    newsockfd = acceptConnection(sockfd);
    if (newsockfd < 0) 
    {
        cleanup(newsockfd, sockfd);
        LOG("ERROR on accept")
        return(4);
    }

    while(1)
    {
        retVal = getRequest(getNetworkByte, newsockfd, buffer, (size_t) sizeof(buffer));
        if (retVal <= 0)
        {
            close(newsockfd);
            if(context->single_scenario)
            {
                break;
            }
            newsockfd = acceptConnection(sockfd);
            if (newsockfd < 0) 
            {
                cleanup(newsockfd, sockfd);
                LOG("ERROR on accept-2")
                return(5);
            }
            continue;
        }
        LOG(buffer)

        handleRequest(buffer, context);

        /* Write a response to the client */
        size_t len = strlen(buffer);
        sent = send(newsockfd, buffer, len, 0);
        if (sent != len)
        {
            cleanup(newsockfd, sockfd);
            LOG ("ERROR on write")
            return(6);
        }
    }
    cleanup(newsockfd, sockfd);
    return 0; 
}

ssize_t getNetworkByte(int socket, char *buffer)
{
    return(recv(socket, buffer, 1, 0));
}

ssize_t getRequest(net_reader reader, int socket, char *buffer, ssize_t len)
{
    bzero(buffer, len);

    ssize_t count = len;
    ssize_t rc;

    while(count > 0)
    {
        rc = reader(socket, buffer);
        if (rc < 0)
        {
            return(-1);
        }
        if(rc == 0)
        {
            len -= count;
            break;
        }
        if((rc == 0) || (*buffer == '\n'))
        {
            *buffer = 0;
            len -= count;
            break;
        }
        buffer += rc;
        count -= rc;
    }
    return(len);
}

char *handle_callback(wire_feature_callback callback, wire_context *context)
{
    if(!context)
    {
        return "[\"fail\",{\"message\":\"no context specified\"}]\n";
    }
    if(callback && ((*callback) (context) != 0))
    {
        return "[\"fail\",{\"message\":\"handler failed\"}]\n";
    }
    return "[\"success\"]\n";
}

void cleanup(int socket1, int socket2)
{
    close(socket1);
    close(socket2);
}

int acceptConnection(int sockfd)
{
    struct sockaddr_in cli_addr;

    socklen_t clilen = sizeof(cli_addr);
    return accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
}

int makeSocket(int port)
{
    int retVal;
    int sockfd;
    struct sockaddr_in serv_addr;

    /* First call to socket() function */
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        return(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
 
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    retVal = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (retVal < 0)
    {
        return(2);
    }

    retVal = listen(sockfd, 5);
    if (retVal < 0)
    {
        close(sockfd);
        return(3);
    }
    return(sockfd);
}

