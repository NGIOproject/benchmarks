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
 * Network transfer codes.
 * This is the server part.
 */
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>

#include "utils.h"
#include "level0.h"

int netmain(char *choice, unsigned long iter)
{
  int retval = 0;
  void* status;
  pthread_mutex_init(&mutex, NULL);
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  /*
   * This piece of code fixes the threads to specific processors (cores)
   * namely, the last two cores.
   *
   * Don't need to call CPU_FREE as we us statically allocated cpu sets.
   */
  cpu_set_t cpuset1, cpuset2;
  CPU_ZERO(&cpuset1);
  CPU_ZERO(&cpuset2);
  int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
  CPU_SET(num_cores-1, &cpuset1);
  CPU_SET(num_cores-2, &cpuset2);


  if (iter == ULONG_MAX){
    iter = 1000;
  }

  if (strcmp("TCP", choice) == 0 || strcmp("+", choice) == 0){
    pthread_create(&callThd[0], &attr, tcp_server, (void *)&iter);
    pthread_create(&callThd[1], &attr, tcp_client, (void *)&iter);
    pthread_setaffinity_np(callThd[0], sizeof(cpu_set_t), &cpuset1);
    pthread_setaffinity_np(callThd[1], sizeof(cpu_set_t), &cpuset2);
    pthread_attr_destroy(&attr);

    /* Wait on the other threads */
    pthread_join(callThd[0], &status);
    pthread_join(callThd[1], &status);
    retval = 0;
  }

  if (strcmp("UDP", choice) == 0){
    pthread_create(&callThd[0], &attr, udp_server, (void *)&iter);
    pthread_create(&callThd[1], &attr, udp_client, (void *)&iter);
    pthread_setaffinity_np(callThd[0], sizeof(cpu_set_t), &cpuset1);
    pthread_setaffinity_np(callThd[1], sizeof(cpu_set_t), &cpuset2);
    pthread_attr_destroy(&attr);

    /* Wait on the other threads */
    pthread_join(callThd[0], &status);
    pthread_join(callThd[1], &status);
    retval = 0;
  }


  return(retval);
}

void* tcp_server(void* it){

  int *iter = (int*)it;
  int i = 0;
  int socket_fd = 0;
  struct sockaddr_in serv_addr;
  size_t buf_size = sizeof(struct timespec);
  char buf[buf_size];



  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1){
    printf("TCP Server Error: Unable to create socket fd\n");
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(9001);
  if (inet_aton("127.0.0.1", &serv_addr.sin_addr)==0) {
    printf("inet_aton() failed\n");
  }

  pthread_mutex_lock(&mutex);
  while(!flag_start){
    pthread_cond_wait(&cond, &mutex);
  }
  /* Spin loop whilst the client sets up and binds the sockets */
  if (connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
    printf("Failed to connect to server\n");
  }
  else{
    printf("Connected successfully\n");
  }
  pthread_mutex_unlock(&mutex);

  int count = 0;
  bzero(buf, buf_size);

  while(i<*iter){
    clock_gettime(CLOCK, (struct timespec*)buf);
    write(socket_fd, buf, buf_size);
    if(count < 0){
      printf("Failed writing rquested bytes to server\n");
    }
    i++;
  }

  close(socket_fd);
  pthread_exit((void*) 0);
}

void* udp_server(void* it){

  int *iter = (int*)it;
  int i = 0;
  struct sockaddr_in si_other;
  int s, slen=sizeof(si_other);
  size_t buf_size = sizeof(struct timespec);
  char buf[buf_size];
  bzero(buf, buf_size);

  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){
    printf("Server socket error");
  }

  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(9001);
  if (inet_aton("127.0.0.1", &si_other.sin_addr)==0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }


  pthread_mutex_lock(&mutex);
  while(!flag_start){
    pthread_cond_wait(&cond, &mutex);
  }
  /* Spin loop whilst the client sets up and binds the sockets */
  pthread_mutex_unlock(&mutex);

  while(i<*iter){
    /*
     * Buffered data transfere does not require locking + signalling
     */
    clock_gettime(CLOCK, (struct timespec *)buf);
    if (sendto(s, buf, buf_size, 0, (const struct sockaddr *)&si_other, slen)==-1){
      perror("sendto()");
    }
    i++;
  }

  close(s);
  pthread_exit((void*) 0);
}
