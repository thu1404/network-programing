#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        struct addrinfo* addr = NULL;
        getaddrinfo(argv[1], "http", NULL, &addr);
        struct addrinfo* root = addr;
        char* str = NULL;

        while (addr != NULL)
        {
            if (addr->ai_family == AF_INET)
            {
                struct sockaddr_in si;
                memcpy(&si, addr->ai_addr, addr->ai_addrlen);
                str = inet_ntoa(si.sin_addr);  
                printf("%s\n", str);
                addr = addr->ai_next;      
                break;
            }else
                addr = addr->ai_next;      
        }

        if (root != NULL)
            freeaddrinfo(root);

        //IP is now available 
        int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sfd >= 0)
        {
            struct sockaddr_in saddr;
            saddr.sin_family = AF_INET;
            saddr.sin_port = htons(80);
            saddr.sin_addr.s_addr = inet_addr(str);
            int result = connect(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
            if (result >= 0)
            {
                char* welcome = "Hello\n";
                result = send(sfd, welcome, strlen(welcome), 0);
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                result = recv(sfd, buffer, sizeof(buffer), 0);
                printf("Received %d bytes\n", result);
                printf("%s\n", buffer);
                close(sfd);
            }
        }
    }
}