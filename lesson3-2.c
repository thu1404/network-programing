#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

// gethostbyname

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        struct hostent* host = gethostbyname(argv[1]);
        if (host != NULL)
        {
            if (host->h_length == 4) //IPv4
            {
                int i = 0;
                while (host->h_addr_list[i] != NULL)
                {
                    unsigned char b1 = host->h_addr_list[i][0];
                    unsigned char b2 = host->h_addr_list[i][1];
                    unsigned char b3 = host->h_addr_list[i][2];
                    unsigned char b4 = host->h_addr_list[i][3];
                    printf("%u.%u.%u.%u\n", b1, b2, b3, b4);
                    i++;
                }
            }
        }
    }
}