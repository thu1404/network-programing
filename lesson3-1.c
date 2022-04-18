#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
// #include <Winsock2.h>

// getaddrinfo
int main(int argc, char** argv)
{
    if (argc > 1)
    {
        struct addrinfo* addr = NULL;
        getaddrinfo(argv[1], "http", NULL, &addr);
        struct addrinfo* root = addr;

        while (addr != NULL)
        {
            if (addr->ai_family == AF_INET)
            {
                struct sockaddr_in si;
                memcpy(&si, addr->ai_addr, addr->ai_addrlen);
                char* str = inet_ntoa(si.sin_addr);  
                printf("%s\n", str);
                addr = addr->ai_next;      
            }else
                addr = addr->ai_next;      
        }

        if (root != NULL)
            freeaddrinfo(root);
    }
}