/* run using ./server <port> */
#include "http_server.hh"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

#include <netinet/in.h>

#include <pthread.h>
#include <cstddef>
#include <cstring>
#include <queue>

/* create a shared buffer */
queue<int> queue1;

/* Array of thread created */
pthread_t thread_id[4];

/* intialize lock */
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void error(char *msg)
{
  perror(msg);
  exit(1);
}

void *start_function(void *arg)
{
  int length;

  int j = *((int *)arg);

  int my_arg;
  while (1)
  {
    pthread_mutex_lock(&lock);
    while (queue1.empty())
    {
      pthread_cond_wait(&cond, &lock);
    }
    my_arg = queue1.front();
    queue1.pop();
    pthread_mutex_unlock(&lock);
    cout << "===========Thread No===========:" << j << endl;
    int n;
    char buffer[1024];
    // printf("My_args value:: %d\n", my_arg);

    /* read message from client */
    bzero(buffer, 1024);

    n = read(my_arg, buffer, 1023);

    if (n < 0)
      error("ERROR reading from socket");
    printf("Here is the message: %s", buffer);

    HTTP_Response *response = handle_request(buffer);
    string str = response->get_string();
    const char *x = str.c_str();
    length = str.size();

    /* send message to the client */
    n = write(my_arg, x, length);
    if (n < 0)
      error("ERROR writing to socket");
  }

  /* send signal to the master thread */
  pthread_cond_signal(&cond);

  close(my_arg);
  pthread_exit(0);
}

int main(int argc, char *argv[])
{
  int sockfd, newsockfd, portno, *new_sock;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;

  if (argc < 2)
  {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  /* create socket */

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
   */

  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind socket to this port number on this machine */

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");

  /* listen for incoming connection requests */

  listen(sockfd, 5);
  clilen = sizeof(cli_addr);

  /* create all the threads at once */
  for (int j = 0; j < 30; j++)
  {
    if (pthread_create(&thread_id[j], NULL, start_function, &j) < 0)
    {
      printf("thread cant be created");
    }
  }

  while (1)
  {
    // printf("Entering while loop\n");

    /* Accept all incoming connections*/

    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

    cout << "================print newsockfd=================:" << newsockfd << endl;

    if (newsockfd < 0)
    {
      error("ERROR on accept");
    }

    /* locks */
    pthread_mutex_lock(&lock);

    /* check the status */
    // cout << "check the status:"<< queue1.size() << endl;

    /* loop into queue when size is raised */
    while (queue1.size() == 20)
    {
      cout << "Queue is FULL";
      pthread_cond_wait(&cond, &lock);
    }

    queue1.push(newsockfd);
    cout << "--------print size in master thread:------" << queue1.size() << endl;

    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&lock);
  }
  printf("levaving while loop\n");
  return 0;
}
