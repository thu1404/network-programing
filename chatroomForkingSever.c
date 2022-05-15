#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/mman.h>

typedef struct tabela
{
  char name[50];
  int PID;
} tab;

static tab *tabelaPersoane;

static int *nrPersoaneConectate;

void afisareConectati()
{
  int i;
  if (*nrPersoaneConectate > 0)
  {
    printf("Numarul de persoane conectate: %d", *nrPersoaneConectate);
    printf("\nPersoane conectate:\n");
    for (i = 0; i < *nrPersoaneConectate; i++)
    {
      printf("%s", tabelaPersoane[i].name);
    }
    printf("\n");
  }
  else
    printf("Niciun client conectat!\n");
}

void doprocessing(int sock, char *numeClient)
{
  //printf("doprocessing id=%d\n",sock);
  int n, i;
  char buffer[256], mesaj[256];
  while (1)
  {
    //printf("%d\n",nrPersoaneConectate);
    bzero(buffer, 256);

    n = read(sock, buffer, 255);
    if (strncmp(buffer, "exit", strlen("exit")) == 0)
    {
      for (i = 0; i < *nrPersoaneConectate; i++)
      {
        if (strcmp(numeClient, tabelaPersoane[i].name) == 0)
          if (i != *nrPersoaneConectate - 1)
          {
            tabelaPersoane[i] = tabelaPersoane[*nrPersoaneConectate - 1];
            //printf("%d\n",nrPersoaneConectate);
          }
      }
      (*nrPersoaneConectate)--;
      printf("S-a deconectat %s", numeClient);
      afisareConectati();
      break;
    }
    if (n < 0)
    {
      perror("ERROR reading from socket");
      exit(1);
    }
    if (strcmp(buffer, "") != 0)
    {
      bzero(mesaj, 256);
      strncpy(mesaj, numeClient, strlen(numeClient) - 1);
      strcat(mesaj, ": ");
      strncat(mesaj, buffer, strlen(buffer));
      printf("%s", mesaj);
    }
    n = write(sock, "I got your message", 18);

    if (n < 0)
    {
      perror("ERROR writing to socket");
      exit(1);
    }
  }
}

int main(int argc, char *argv[])
{
  nrPersoaneConectate = mmap(NULL, sizeof *nrPersoaneConectate,
      PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  tabelaPersoane = (tab *) mmap(0, sizeof(tab), PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  int sockfd, newsockfd, portno, clilen;
  char buffer[256], mesaj[256];
  char buffer_pid[6];
  struct sockaddr_in serv_addr, cli_addr;
  int n, i;
  *nrPersoaneConectate = 0;
  /* First call to socket() function */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0)
  {
    perror("ERROR opening socket");
    exit(1);
  }

  /* Initialize socket structure */
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = 51100;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* Now bind the host address using bind() call.*/
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
  {
    perror("ERROR on binding");
    exit(1);
  }

  /* Now start listening for the clients, here
   * process will go in sleep mode and will wait
   * for the incoming connection
   */

  if (listen(sockfd, 5) < 0)
  {
    printf("error ...");
    exit(1);
  }
  clilen = sizeof(cli_addr);

  while (1)
  {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    //printf("sockfd%d\n",sockfd);
    //printf("newsockfd%d\n",newsockfd);
    if (newsockfd < 0)
    {
      perror("ERROR on accept");
      exit(1);
    }
    (*nrPersoaneConectate)++;
    /* Create child process */
    int pid = fork();
    if (pid < 0)
    {
      perror("ERROR on fork");
      exit(1);
    }

    if (pid == 0)
    {

      /* This is the client process */
      close(sockfd);
      if (write(newsockfd, "Name: ", sizeof("Name: ")) < 0)
      {
        printf("Error on sending \"Name: \" string.");
        exit(1);
      }
      if (read(newsockfd, buffer, 255) < 0)
      {
        printf("Error on reading the name response.");
        exit(1);
      }
      strcpy(tabelaPersoane[(*nrPersoaneConectate) - 1].name, buffer);
      tabelaPersoane[(*nrPersoaneConectate) - 1].PID = getpid();
      bzero(mesaj, 256);
      strcpy(mesaj, "S-a conectat ");
      strncat(mesaj, buffer, strlen(buffer) - 1);
      strcat(mesaj, " PID: ");
      sprintf(buffer_pid, "%d", tabelaPersoane[(*nrPersoaneConectate) - 1].PID);
      strcat(mesaj, buffer_pid);
      strcat(mesaj, "\n");
      printf("%s", mesaj);
      afisareConectati();
      doprocessing(newsockfd, buffer);
      exit(0);
    }
    else
    {

      close(newsockfd);
      //printf("Close\n");         
    }
  } /* end of while */
}