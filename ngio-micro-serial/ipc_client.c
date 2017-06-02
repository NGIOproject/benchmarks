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
 * Example of IPC using Shared Memory Segment.
 * This is the client part.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "utils.h"
#include "level0.h"


void *shm_client(void* arg){

  unsigned int iter = 0;
  int payload_sz = 0;
  struct ipc_thread_info* tinfo = arg;
  iter = tinfo->iteration;
  payload_sz = tinfo->payload_size * 1024; /* In KB */

  int shmid;
  key_t key;
  char *shm;
  int sz, r, i;
  i = 0;
  struct timespec start, end;
  struct timespec b, d;
  sz = sizeof(struct timespec);
  r = 0;

  /*
   * We need to get the segment named
   * "5678", created by the server.
   */
  key = 5678;

  pthread_mutex_lock(&mutex);
  while(!flag_start){
    pthread_cond_wait(&cond, &mutex);
  }
  /*
   * Locate the segment.
   */
  if ((shmid = shmget(key, sz+payload_sz, 0666)) < 0) {
    perror("shmget");
  }

  /*
   * Now we attach the segment to our data space.
   */
  if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
    perror("shmat");
  }
  pthread_mutex_unlock(&mutex);


  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  memset(overheads, 0, 2 * 1000 * sz);
  if (overheads == NULL){
    perror("SHM overheads alloc fail");
  }
  volatile int j = 0;
  while(j<1000){
    clock_gettime(CLOCK, &overheads[2*j]);
    clock_gettime(CLOCK, &b);
    memcpy(&d, &b, sz);
    clock_gettime(CLOCK, &overheads[2*j+1]);
    j++;
  }

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * iter * sz);
  if (results == NULL){
    perror("SHM results alloc fail");
  }
  memset(results, 0, 2 * iter * sz);

  char* payload_buf = (char *)malloc(payload_sz);
  memset(payload_buf, 0, payload_sz);

  i = 0;
  while(i<iter){
    pthread_mutex_lock(&mutex);
    p_flag = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_lock(&mutex);
    while(p_flag){
      pthread_cond_wait(&cond, &mutex);
    }
    memcpy(&results[2*i], shm, sz);
    memcpy(payload_buf, shm+sz, payload_sz);
    clock_gettime(CLOCK, &results[2*i+1]);

    pthread_mutex_unlock(&mutex);
    i++;
  }


  discrete_elapsed_hr(overheads, results, &iter, "SHM");
  shmdt(shm);
  free(results);
  free(overheads);
  free(payload_buf);
  pthread_exit((void*) 0);
}



/*
 * Client-side FIFO
 *
 * This will loop 'iter' times reading a timestamp from the FIFO pipe and then
 * printing the difference between it and the current time in microseconds.
 *
 *  Inputs: int iter - the desired number of iterations
 * Outputs: differences between timestamps to stdout
 * Returns: 1 in the case of a failure to open FIFO pipe
 *          0 otherwise
 */
void* fifo_client(void* arg){

  FILE *fp;
  unsigned int iter = 0;
  int payload_sz = 0;
  struct ipc_thread_info* tinfo = arg;
  iter = tinfo->iteration;
  payload_sz = tinfo->payload_size * 1024; /* In KB */

  int sz, i;
  sz = sizeof(struct timespec);
  i = 0;
  struct timespec start, end;

  pthread_mutex_lock(&mutex);
  while(!flag_start){
    pthread_cond_wait(&cond, &mutex);
  }

  /* Open the pipe if it exists */
  if((fp = fopen("FIFO_FILE", "r")) == NULL) {
    perror("fopen for reading");
  }

  pthread_mutex_unlock(&mutex);

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * iter * sz);
  memset(results, 0, 2 * iter * sz);
  if (results == NULL){
    perror("FIFO results alloc fail");
  }
  char* payload_buf = (char *)malloc(payload_sz);
  memset(payload_buf, 0, payload_sz);

  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  if (overheads == NULL){
    perror("FIFO overheads alloc fail");
  }
  memset(overheads, 0, 2 * 1000 * sz);

  struct timespec b, d;
  volatile int j = 0;
  while(j<1000){
    clock_gettime(CLOCK, &overheads[2*j]);
    clock_gettime(CLOCK, &b);
    memcpy(&d, &b, sz);
    clock_gettime(CLOCK, &overheads[2*j+1]);
    j++;
  }


  /* Main loop */
  while(i<iter){
    fread(&results[2*i], sz, 1, fp);
    fread(payload_buf, payload_sz, 1, fp);
    clock_gettime(CLOCK, &results[2*i+1]);
    i++;
  }



  discrete_elapsed_hr(overheads, results, &iter, "IPC - FIFO");
  free(payload_buf);
  free(overheads);
  free(results);

  fclose(fp);
  pthread_exit((void*) 0);
}

/*
 * Client-side UNIX socket
 *
 * This will loop 'iter' times reading a timestamp from the socket and then
 * printing the difference between it and the current time in microseconds.
 *
 *  Inputs: int iter - the desired number of iterations
 * Outputs: differences between timestamps to stdout
 * Returns: 1 in the case of a failure to create or connect to a socket
 *          0 otherwise
 */
void* socket_client(void* arg){

  FILE *fp;
  unsigned int iter = 0;
  int payload_sz = 0;
  struct ipc_thread_info* tinfo = arg;
  iter = tinfo->iteration;
  payload_sz = tinfo->payload_size * 1024; /* In KB */

  int sz, r, i, sp;
  int s, t, len;
  struct sockaddr_un remote;

  sz = sizeof(struct timespec);
  i = 0;
  struct timespec start, end;


  if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket");
  }

  pthread_mutex_lock(&mutex);
  while(!flag_start){
    pthread_cond_wait(&cond, &mutex);
  }
  remote.sun_family = AF_UNIX;
  strcpy(remote.sun_path, "SOCK_PATH");
  len = strlen(remote.sun_path) + sizeof(remote.sun_family);
  if (connect(s, (struct sockaddr *)&remote, len) == -1) {
    perror("connect");
  }
  pthread_mutex_unlock(&mutex);

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * iter * sz);
  if (results == NULL){
    perror("SOCKET results alloc fail");
  }
  memset(results, 0, 2 * iter * sz);
  char* payload_buf = (char *)malloc(payload_sz+sz);
  memset(payload_buf, 0, payload_sz+sz);

  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  if (overheads == NULL){
    perror("SOCKET overheads alloc fail");
  }
  memset(overheads, 0, 2 * 1000 * sz);

  struct timespec b, d;
  volatile int j = 0;
  while(j<1000){
    clock_gettime(CLOCK, &overheads[2*j]);
    clock_gettime(CLOCK, &b);
    memcpy(&d, &b, sz);
    clock_gettime(CLOCK, &overheads[2*j+1]);
    j++;
  }

  char* rbuf = (char*)malloc(102400);
  char* rb_p = rbuf;
  int tc = 0;

  while(i<iter){
    tc = 0;
    while (tc < payload_sz+sz){
      if ((t=recv(s, rbuf, 102400, 0)) > 0){
        memcpy(payload_buf+tc, rbuf, t);
        tc += t;
      }
    }

    memcpy(&results[2*i], payload_buf, sz);
    clock_gettime(CLOCK, &results[2*i+1]);
    i++;
  }

  discrete_elapsed_hr(overheads, results, &iter, "IPC - SOCK");
  free(payload_buf);
  free(overheads);
  free(results);
  free(rbuf);
  close(s);
  pthread_exit((void*) 0);
}
