#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<signal.h>
#include<sys/wait.h>
int ppid = 0;
int client[1024];
int count = 0;
void signal_int(){
    FILE *f=NULL;
    f = fopen("message.txt","r");
    char* message;
    long    numbytes;
    fseek(f, 0L, SEEK_END);
    numbytes = ftell(f);
    fseek(f, 0L, SEEK_SET); 
    message = (char*)calloc(numbytes, sizeof(char));
    fread(message, sizeof(char), numbytes, f);
    fclose(f);
    printf("%s\n",message);
    f = fopen("sender.txt","r");
    int cfd = getw(f);
    fclose(f);
    printf("%d\n",cfd);
    int i;
    for(i=0;i<count;i++){
        if(client[i] != cfd){
            send(client[i],message,strlen(message),0);
        }            
    }
    printf("A child process received a message \n");
}
void signal_handler(){
    int stat = 0;
    while (waitpid(-1,&stat,WNOHANG) > 0)
    {
        printf("A child process has been terminated\n");
    }
    
}
void echo(int cfd){
    FILE *f=NULL;
    char buffer[1024];
    while (0 == 0)
    {
        memset(buffer,0,sizeof(buffer));
        printf("Waiting for data from %d\n",cfd);
        int r = recv(cfd,buffer,sizeof(buffer),0);
        if(r>0){
            f = fopen("message.txt","w");
            printf("%d: %s\n",getpid(),buffer);
            fputs(buffer,f);
            fclose(f);
            f = fopen("sender.txt","w");
            putw(cfd,f);
            fclose(f);
            kill(ppid,SIGINT);
        }else{
            printf("%d closed because a client has disconnected\n",cfd);
            close(cfd);
            break;
        }
    }
    printf("A child process has been terminated: %d\n",getpid());
}

int	main(int argc, char **argv)
{
    int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in saddr;
    struct sockaddr caddr; //Bien ra chua dia chi client noi den
    int clen = sizeof(caddr); //Bien vao + ra chua so byte duoc ghi vao caddr
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = 0;
    bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
    listen(sfd, 10);
    ppid = getpid();
    signal(SIGINT,signal_int);
    signal(SIGCHLD,signal_handler);
    while (0 == 0)
    {
        int cfd = accept(sfd, (struct sockaddr*)&caddr, &clen);
        printf("%d openned\n",cfd);
        if(cfd>0){
            if(fork() == 0){
                //child process 
                close(sfd);
                echo(cfd);
                close(cfd);
                exit(0);
            }else{
                //parent process
                client[count++] = cfd;
            }
        }
    }
    // int i = 0;
    // for(i=0;i<10;i++){
    //     int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //     if (sfd >= 0)
    //     {
    //         struct sockaddr_in saddr;
    //         saddr.sin_family = AF_INET;
    //         saddr.sin_port = htons(8000);
    //         saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //         int result = connect(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
    //     }
    // }
    while(0==0);
    return 0;
}
