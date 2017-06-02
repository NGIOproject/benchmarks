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
 * Example of IPC using Shared Memory Segment
 * This is the server part.
 */

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>
#include <sched.h>

#include "utils.h"
#include "level0.h"

int ipcmain(char *choice, unsigned long iter, unsigned int size)
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

  struct ipc_thread_info tinfo;
  tinfo.iteration = iter;
  tinfo.payload_size = size;

  if (strcmp("SHM", choice) == 0 || strcmp("+", choice) == 0){
    pthread_create(&callThd[0], &attr, shm_server, (void *)&tinfo);
    pthread_create(&callThd[1], &attr, shm_client, (void *)&tinfo);
    pthread_setaffinity_np(callThd[0], sizeof(cpu_set_t), &cpuset1);
    pthread_setaffinity_np(callThd[1], sizeof(cpu_set_t), &cpuset2);
    pthread_attr_destroy(&attr);

    /* Wait on the other threads */
    pthread_join(callThd[0], &status);
    pthread_join(callThd[1], &status);
    retval = 0;
  }

  if (strcmp("FIFO", choice) == 0){
    pthread_create(&callThd[0], &attr, fifo_server, (void *)&tinfo);
    pthread_create(&callThd[1], &attr, fifo_client, (void *)&tinfo);
    pthread_setaffinity_np(callThd[0], sizeof(cpu_set_t), &cpuset1);
    pthread_setaffinity_np(callThd[1], sizeof(cpu_set_t), &cpuset2);
    pthread_attr_destroy(&attr);

    /* Wait on the other threads */
    pthread_join(callThd[0], &status);
    pthread_join(callThd[1], &status);
    retval = 0;
  }

  if (strcmp("SOCK", choice) == 0){
    pthread_create(&callThd[0], &attr, socket_server, (void *)&tinfo);
    pthread_create(&callThd[1], &attr, socket_client, (void *)&tinfo);
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

void* shm_server(void* arg){

  int iter = 0;
  int payload_sz = 0;
  struct ipc_thread_info* tinfo = arg;
  iter = tinfo->iteration;
  payload_sz = tinfo->payload_size * 1024; /* In KB */

  char c;
  int shmid, shmid_sp;
  key_t key;
  char *shm;
  int sz, i, j;
  struct timespec tv, *tvp;
  i = 0;

  /*
   * We'll name our shared memory segment
   * "5678".
   */
  key = 5678;

  /*
   * Compute size of timeval
   */
  sz = sizeof(struct timespec);
  tvp = &tv;


  pthread_mutex_lock(&mutex);
  if (!flag_start){
    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, sz+payload_sz, IPC_CREAT | 0666)) < 0) {
      perror("server shmget");
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
      perror("server shmat");
    }
    flag_start = 1;
    pthread_cond_signal(&cond);
  }
  pthread_mutex_unlock(&mutex);

  char* payload_buf = (char*)malloc(payload_sz);
  memset(payload_buf, 0, payload_sz);


  while(i<iter){
    pthread_mutex_lock (&mutex);
    while(!p_flag){
      pthread_cond_wait(&cond, &mutex);
    }
    for (j=0;j<tinfo->payload_size;j++){
      payload_buf[j] = rand() % 255;
    }
    clock_gettime(CLOCK, tvp);
    memcpy(shm, tvp, sz);
    memcpy(shm+sz, payload_buf, payload_sz);
    p_flag = 0;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock (&mutex);

    /* printf("Server unlocked\n"); */
    i++;
  }


  shmdt(shm);
  shmctl(shmid, IPC_RMID, NULL);
  pthread_exit((void*) 0);
}



/*
 * Server-side FIFO
 *
 * This will loop 'iter' times writing a timestamp to the FIFO pipe.
 * Should the pipe not exist initially, it will be created.
 *
 *  Inputs: int iter - the desired number of iterations
 * Outputs: none
 * Returns: 1 in the case of a failure to open FIFO pipe or create it
 *          0 otherwise
 */
void* fifo_server(void* arg){

  FILE *fp;
  int iter = 0;
  int payload_sz = 0;
  struct ipc_thread_info* tinfo = arg;
  iter = tinfo->iteration;
  payload_sz = tinfo->payload_size * 1024; /* In KB */

  struct timespec tm, *tm_p;
  int sz, i, retval;
  tm_p = &tm;
  sz = sizeof(struct timespec);
  i = 0;

  umask(0);
  /* Create the FIFO if it does not exist */
  pthread_mutex_lock(&mutex);
  if (!flag_start){
    if ((retval = mkfifo("FIFO_FILE", S_IFIFO|0666)) != 0){
      perror("error creating FIFO");
    }

    flag_start = 1;
    pthread_cond_signal(&cond);
  }
  pthread_mutex_unlock(&mutex);

  if ((fp = fopen("FIFO_FILE", "w")) == NULL){
    perror("fopen for writing");
  }

  char* payload_buf = (char*)malloc(payload_sz + sz);
  memset(payload_buf, 0, payload_sz+sz);
  int j=0;
  

  /* Main loop */
  /* This is bit we measure from*/
  while(i<iter){
    for (j=0;j<payload_sz;j++){
      *(payload_buf+sz+j) = rand() % 255;
    }
    clock_gettime(CLOCK, (struct timespec *)payload_buf);
    fwrite(payload_buf, sz+payload_sz, 1, fp);
    i++;
  }

  fclose(fp);
  unlink("FIFO_FILE");
  pthread_exit((void*) 0);
}

/*
 * Server-side UNIX socket
 *
 * This will loop 'iter' times writing a timestamp to the socket.
 *
 *  Inputs: int iter - the desired number of iterations
 * Outputs: none
 * Returns: 1 in the case of a failure to open FIFO pipe or create it
 *          0 otherwise
 */

void* socket_server(void* arg){

  int iter = 0;
  int payload_sz = 0;
  struct ipc_thread_info* tinfo = arg;
  iter = tinfo->iteration;
  payload_sz = tinfo->payload_size * 1024; /* In KB */

  int sz, i, retval;
  int s, s2, t, len;
  struct sockaddr_un local, remote;
  sz = sizeof(struct timespec);
  i = 0;
  pthread_mutex_lock(&mutex);
  if (!flag_start){
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(1);
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, "SOCK_PATH");
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(s, (struct sockaddr *)&local, len) == -1) {
      perror("bind");
      exit(1);
    }

    if (listen(s, 5) == -1) {
      perror("listen");
      exit(1);
    }
    flag_start = 1;
    pthread_cond_signal(&cond);
  }
  pthread_mutex_unlock(&mutex);

  t = sizeof(remote);
  if ((s2 = accept(s, (struct sockaddr *)&remote, (socklen_t *)&t)) == -1) {
    perror("accept");
    exit(1);
  }
  char* payload_buf = (char*)malloc(payload_sz + sz);
  memset(payload_buf, 0, payload_sz+sz);
  int j=0;


  while(i<iter){
    for (j=0;j<payload_sz;j++){
      *(payload_buf+sz+j) = rand() % 255;
    }
    clock_gettime(CLOCK, (struct timespec *)payload_buf);
    send(s2, payload_buf, payload_sz+sz, 0);
    i++;
  }

  close(s2);
  close(s);
  unlink(local.sun_path);
  free(payload_buf);

  pthread_exit((void*) 0);
}
