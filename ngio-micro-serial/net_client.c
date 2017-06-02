/* Copyright (c) 2015 The University of Edinburgh. */

/* 
* This software was developed as part of the                       
* EC FP7 funded project Adept (Project ID: 610490)                 
* www.adept-project.eu                                            
*/

/* Licensed under the Apache License, Version 2.0 (the "License"); */
/* you may not use this file except in compliance with the License. */
/* You may obtain a copy of the License at */

/*     http://www.apache.org/licenses/LICENSE-2.0 */

/* Unless required by applicable law or agreed to in writing, software */
/* distributed under the License is distributed on an "AS IS" BASIS, */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. */
/* See the License for the specific language governing permissions and */
/* limitations under the License. */

/*
 * Example of Network Transfer.
 * This is the client part.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "utils.h"
#include "level0.h"


void *tcp_client(void* it){

  unsigned int* iter = (unsigned int*)it;
  int i = 0;

  /* Create sockets and the like here */

  int socket_fd = 0;
  int newsocket_fd = 0;
  int port_number = 0;
  int retval = 0;
  int n = 0;

  socklen_t clilen = 0;
  size_t buf_size = sizeof(struct timespec);
  char buf[buf_size];


  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;

  /* Get file descriptor for listening socket */
  socket_fd = socket(AF_INET, SOCK_STREAM, 0); /* Use default protocol (TCP) */
  if (socket_fd < 0){
    printf("TCP Client Error: Unable to create socket\n");
  }

  bzero((char *)&serv_addr, sizeof(serv_addr)); /* Ensure structure representing server socket is flushed to 0 */
  port_number = 9001; /* Hard code this for disambiguation, easy record-keeping */

  /*
   * Setup structure representing listening socket
   * We accept any connection on our chosen port (9001) from any remote IP
   */
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port_number);

  bzero(buf, buf_size); /* Zero out buffer */

  /* Bind to the local socket */
  retval = bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (retval < 0){
    printf("TCP Client Error: Unable to bind to local socket\n");
  }


  struct timespec b, d, result;
  int r = 0;

  size_t sz = sizeof(struct timespec);
  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  if (overheads == NULL){
    perror("TCP overheads alloc fail");
  }
  memset(overheads, 0, 2 * 1000 * sz);

  volatile int j = 0;
  while(j<1000){
    clock_gettime(CLOCK, &overheads[2*j]);
    clock_gettime(CLOCK, &b);
    memcpy(&d, &b, sz);
    n = j;
    if (n<0){
      printf("Dummy statement\n");
    }
    clock_gettime(CLOCK, &overheads[2*j+1]);
    j++;
  }

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * *iter * sz);
  if (results == NULL){
    perror("TCP results alloc fail");
  }
  memset(results, 0, 2 * *iter * sz);



  pthread_mutex_lock(&mutex);
  if (!flag_start){
    /*
     * This stops the other end connecting until such times as we are ready to listen.
     */
    flag_start = 1;
    pthread_cond_signal(&cond);
  }
  pthread_mutex_unlock(&mutex);


  listen(socket_fd, 5);
  clilen = sizeof(cli_addr);

  newsocket_fd = accept(socket_fd, (struct sockaddr *)&cli_addr, &clilen);
  if (newsocket_fd < 0){
    printf("TCP Client Error: Unable to accept connections\n");
  }


  for (i=0;i<*iter;i++){
    n = read(newsocket_fd, (void*)&results[2*i], buf_size);
    clock_gettime(CLOCK, &results[2*i+1]);
    if (n < 0){
      printf("TCP Client Error: Received null packet\n");
    }
  }

  /* Close sockets */
  close(newsocket_fd);
  close(socket_fd);

  discrete_elapsed_hr(overheads, results, iter, "TCP");
  free(results);
  free(overheads);
  pthread_exit((void*) 0);
}


void *udp_client(void* it){

  unsigned int* iter = (unsigned int*)it;
  int i = 0;
  struct sockaddr_in si_me, si_other;
  int s;
  socklen_t slen=sizeof(si_other);
  size_t buf_size = sizeof(struct timespec);
  char buf[buf_size];
  memset((char *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(9001);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);
  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){
    perror("socket");
  }


  pthread_mutex_lock(&mutex);
  if (!flag_start){
    if (bind(s, (const struct sockaddr *)&si_me, sizeof(si_me))==-1){
      perror("binding");
    }

    flag_start = 1;
    pthread_cond_signal(&cond);
  }
  pthread_mutex_unlock(&mutex);

  struct timespec b,d,result;
  int r = 0;
  size_t sz = sizeof(struct timespec);
  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  if (overheads == NULL){
    printf("UDP overheads alloc fail");
  }
  memset(overheads, 0, 2 * 1000 * sz);

  volatile int j = 0;
  while(j<1000){
    clock_gettime(CLOCK, &overheads[2*j]);
    clock_gettime(CLOCK, &b);
    clock_gettime(CLOCK, &overheads[2*j+1]);
    j++;
  }


  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * *iter * sz);
  if (results == NULL){
    perror("UDP results alloc fail");
  }
  memset(results, 0, 2 * *iter * sz);


  for (i=0;i<*iter;i++){
    if (recvfrom(s, (void*)&results[2*i], buf_size, 0, (struct sockaddr *)&si_other, &slen)==-1){
      perror("recvfrom()");
    }
    clock_gettime(CLOCK, &results[2*i+1]);
  }

  discrete_elapsed_hr(overheads, results, iter, "UDP");

  free(results);
  free(overheads);
  close(s);
  pthread_exit((void*) 0);
}
