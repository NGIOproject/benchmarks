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
 * Adept thread and process benchmarks
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <limits.h>

#include "utils.h"
#include "level0.h"

/*
 * Local fn prototypes
 * These aren't needed anywhere else so not in header
 */
void* pthread_create_fn(void*);
void* pthread_destroy_fn(void*);


int process_pthread_create(unsigned int iter){

  /********************************************************************************************************************/
  /* First we deal with the setup for the pthread */
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
  cpu_set_t cpuset1;
  CPU_ZERO(&cpuset1);
  int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
  CPU_SET(num_cores-1, &cpuset1);

  /* We don't really need to pass this down to the thread, but use it as a dummy arg for more realism */
  struct ipc_thread_info tinfo;
  tinfo.iteration = iter;
  tinfo.payload_size = 0;
  /********************************************************************************************************************/

  /********************************************************************************************************************/
  /* Now we deal with the SHM segment for moving results about */
  int shmid;
  key_t key;
  char *shm;
  key = 5678;
  struct timespec child_end;
  size_t sz = sizeof(struct timespec);
  int i = 0;


  /* Create the segment */
  if ((shmid = shmget(key, sz, IPC_CREAT | 0666)) < 0) {
    perror("server shmget");
  }

  /*
   * Now we attach the segment to our data space.
   */
  if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
    perror("server shmat");
  }
  /********************************************************************************************************************/

  /********************************************************************************************************************/
  /*
   * Now we setup the overheads and results data structures, and measure the overheads
   */
  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  memset(overheads, 0, 2*1000*sz);
  if (overheads == NULL){
    perror("Process::pthread_create overheads allocation failure.");
    exit(1);
  }

  for (i=0;i<1000;i++){
    clock_gettime(CLOCK, &overheads[2*i]);
    struct timespec dummy;
    clock_gettime(CLOCK, &overheads[2*i+1]);
  }

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * iter * sz);
  memset(results, 0, 2*iter*sz);
  if (results == NULL){
    perror("Process::pthread_create results allocation failure.");
    exit(1);
  }
  /********************************************************************************************************************/

  /********************************************************************************************************************/
  /*
   * Now do the measured loop
   */
  for (i=0;i<iter;i++){
    clock_gettime(CLOCK, &results[2*i]);
    pthread_create(&callThd[0], &attr, pthread_create_fn, (void *)&tinfo);
    pthread_setaffinity_np(callThd[0], sizeof(cpu_set_t), &cpuset1);
    pthread_attr_destroy(&attr);

    /* Wait on the thread finishing */
    pthread_join(callThd[0], &status);
    /* Copy the result into results array */
    memcpy(&results[2*i+1], shm, sz);
  }


  /********************************************************************************************************************/

  discrete_elapsed_hr(overheads, results, &iter, "Process::pthread_create");

  shmdt(shm);
  shmctl(shmid, IPC_RMID, NULL);
  free(results);
  free(overheads);
  return 0;
}

int process_pthread_destroy(unsigned int iter){
  /********************************************************************************************************************/
  /* First we deal with the setup for the pthread */
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
  cpu_set_t cpuset1;
  CPU_ZERO(&cpuset1);
  int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
  CPU_SET(num_cores-1, &cpuset1);

  /* We don't really need to pass this down to the thread, but use it as a dummy arg for more realism */
  struct ipc_thread_info tinfo;
  tinfo.iteration = iter;
  tinfo.payload_size = 0;
  /********************************************************************************************************************/

  /********************************************************************************************************************/
  /* Now we deal with the SHM segment for moving results about */
  int shmid;
  key_t key;
  char *shm;
  key = 5678;
  struct timespec child_end;
  size_t sz = sizeof(struct timespec);
  int i = 0;


  /* Create the segment */
  if ((shmid = shmget(key, sz, IPC_CREAT | 0666)) < 0) {
    perror("server shmget");
  }

  /*
   * Now we attach the segment to our data space.
   */
  if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
    perror("server shmat");
  }
  /********************************************************************************************************************/

  /********************************************************************************************************************/
  /*
   * Now we setup the overheads and results data structures, and measure the overheads
   */
  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  memset(overheads, 0, 2*1000*sz);
  if (overheads == NULL){
    perror("Process::pthread_destroy overheads allocation failure.");
    exit(1);
  }

  for (i=0;i<1000;i++){
    clock_gettime(CLOCK, &overheads[2*i]);
    struct timespec dummy;
    clock_gettime(CLOCK, &overheads[2*i+1]);
  }

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * iter * sz);
  memset(results, 0, 2*iter*sz);
  if (results == NULL){
    perror("Process::pthread_destroy results allocation failure.");
    exit(1);
  }
  /********************************************************************************************************************/

  /********************************************************************************************************************/
  /*
   * Now do the measured loop
   */
  for (i=0;i<iter;i++){

    pthread_create(&callThd[0], &attr, pthread_create_fn, (void *)&tinfo);
    pthread_setaffinity_np(callThd[0], sizeof(cpu_set_t), &cpuset1);
    pthread_attr_destroy(&attr);

    /* Wait on the thread finishing */
    pthread_join(callThd[0], &status);
    /* Copy the result into results array */
    clock_gettime(CLOCK, &results[2*i+1]);
    memcpy(&results[2*i], shm, sz);
  }


  /********************************************************************************************************************/

  discrete_elapsed_hr(overheads, results, &iter, "Process::pthread_destroy");

  shmdt(shm);
  shmctl(shmid, IPC_RMID, NULL);
  free(results);
  free(overheads);

  return 0;
}

void* pthread_create_fn(void* arg){
  /* Don't need to do work here */
  struct timespec child_end;
  clock_gettime(CLOCK, &child_end);

  size_t sz = sizeof(struct timespec);
  int shmid;
  key_t key;
  char *shm;
  key = 5678;

  shm = NULL;
  if ((shmid = shmget(key, sz, 0666)) < 0) {
    perror("shmget");
  }
  if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
    perror("shmat");
  }
  memcpy(shm, &child_end, sz);
  pthread_exit((void*) 0);
}

void* pthread_destroy_fn(void* arg){

  size_t sz = sizeof(struct timespec);
  int shmid;
  key_t key;
  char *shm;
  key = 5678;

  shm = NULL;
  if ((shmid = shmget(key, sz, 0666)) < 0) {
    perror("shmget");
  }
  if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
    perror("shmat");
  }
  clock_gettime(CLOCK, (struct timespec*)shm);
  pthread_exit((void*) 0);

}


int process_fork_create(unsigned int iter){

  struct timespec start, end;
  pid_t child_pid;
  int status = 0;
  int i = 0;


  /*
   * In this benchmark, overheads are nil, we are measuring a single instruction
   */
  size_t sz = sizeof(struct timespec);
  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  memset(overheads, 0, 2*1000*sz);
  if (overheads == NULL){
    perror("Process::fork_create overheads allocation failure.");
    exit(1);
  }

  for (i=0;i<1000;i++){
    clock_gettime(CLOCK, &overheads[2*i]);
    clock_gettime(CLOCK, &overheads[2*i+1]);
  }

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * iter * sz);
  memset(results, 0, 2*iter*sz);
  if (results == NULL){
    perror("Process::fork_create results allocation failure.");
    exit(1);
  }


  int shmid;
  key_t key;
  char *shm;
  key = 5678;
  struct timespec child_end;

  /* Create the segment */
  if ((shmid = shmget(key, sz, IPC_CREAT | 0666)) < 0) {
    perror("server shmget");
  }

  /*
   * Now we attach the segment to our data space.
   */
  if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
    perror("server shmat");
  }



  for (i=0;i<iter;i++){
    clock_gettime(CLOCK, &results[2*i]);
    child_pid = fork();
    if (child_pid == 0){
      /* This is the child process. */
      clock_gettime(CLOCK, &child_end);

      shm = NULL;
      if ((shmid = shmget(key, sz, 0666)) < 0) {
        perror("shmget");
      }
      if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
      }
      memcpy(shm, &child_end, sz);
      _exit((int)1);
    }
    else if (child_pid == -1){
      /* This was an error */
      exit(1);
    }
    else{
      /* This is the parent process. */
      waitpid(child_pid, &status, 0);
      memcpy(&results[2*i+1], shm, sz);
    }

  }

  discrete_elapsed_hr(overheads, results, &iter, "Process::fork_create");


  free(results);
  free(overheads);
  shmdt(shm);
  shmctl(shmid, IPC_RMID, NULL);


  return 0;
}

int process_fork_destroy(unsigned int iter){
  struct timespec start, end;
  pid_t child_pid;
  int status = 0;
  int i = 0;


  /*
   * In this benchmark, overheads are nil, we are measuring a single instruction
   */
  size_t sz = sizeof(struct timespec);
  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  memset(overheads, 0, 2*1000*sz);
  if (overheads == NULL){
    perror("Process::fork_create overheads allocation failure.");
    exit(1);
  }

  for (i=0;i<1000;i++){
    clock_gettime(CLOCK, &overheads[2*i]);
    clock_gettime(CLOCK, &overheads[2*i+1]);
  }

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * iter * sz);
  memset(results, 0, 2*iter*sz);
  if (results == NULL){
    perror("Prcoess::fork_create results allocation failure.");
    exit(1);
  }


  int shmid;
  key_t key;
  char *shm;
  key = 5678;
  struct timespec child_start;

  /* Create the segment */
  if ((shmid = shmget(key, sz, IPC_CREAT | 0666)) < 0) {
    perror("server shmget");
  }

  /*
   * Now we attach the segment to our data space.
   */
  if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
    perror("server shmat");
  }



  for (i=0;i<iter;i++){
    child_pid = fork();
    if (child_pid == 0){
      /* This is the child process. */
      shm = NULL;
      if ((shmid = shmget(key, sz, 0666)) < 0) {
        perror("shmget");
      }
      if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
      }
      clock_gettime(CLOCK, &child_start);
      memcpy(shm, &child_start, sz);
      _exit((int)1);
    }
    else if (child_pid == -1){
      /* This was an error */
      exit(1);
    }
    else{
      /* This is the parent process. */
      waitpid(child_pid, &status, 0);
      clock_gettime(CLOCK, &results[2*i+1]);
      memcpy(&results[2*i], shm, sz);
    }

  }

  discrete_elapsed_hr(overheads, results, &iter, "Process::fork_destroy");

  free(results);
  free(overheads);
  shmdt(shm);
  shmctl(shmid, IPC_RMID, NULL);

  return 0;
}

int process_vfork_create(unsigned int iter){
  pid_t child_pid;
  int status = 0;
  int i = 0;


  /*
   * In this benchmark, overheads are nil, we are measuring a single instruction
   */
  size_t sz = sizeof(struct timespec);
  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  memset(overheads, 0, 2*1000*sz);
  if (overheads == NULL){
    perror("Process::vfork_create overheads allocation failure.");
    exit(1);
  }

  for (i=0;i<1000;i++){
    clock_gettime(CLOCK, &overheads[2*i]);
    clock_gettime(CLOCK, &overheads[2*i+1]);
  }

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * iter * sz);
  memset(results, 0, 2*iter*sz);
  if (results == NULL){
    perror("Process::vfork_create results allocation failure.");
    exit(1);
  }


  for (i=0;i<iter;i++){
    clock_gettime(CLOCK, &results[2*i]);
    child_pid = vfork();
    if (child_pid == 0){
      /* This is the child process. */
      clock_gettime(CLOCK, &results[2*i+1]);
      _exit((int)1);
    }
    else if (child_pid == -1){
      /* This was an error */
      exit(1);
    }
    else{
      /* This is the parent process. */
      /* vfork() blocks so we can safely do nothing here */
    }

  }

  discrete_elapsed_hr(overheads, results, &iter, "Process::vfork_create");

  free(results);
  free(overheads);

  return 0;
}

int process_vfork_destroy(unsigned int iter){

  pid_t child_pid;
  int status = 0;
  int i = 0;


  /*
   * In this benchmark, overheads are nil, we are measuring a single instruction
   */
  size_t sz = sizeof(struct timespec);
  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  memset(overheads, 0, 2*1000*sz);
  if (overheads == NULL){
    perror("Process::vfork_destroy overheads allocation failure.");
    exit(1);
  }

  for (i=0;i<1000;i++){
    clock_gettime(CLOCK, &overheads[2*i]);
    clock_gettime(CLOCK, &overheads[2*i+1]);
  }

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * iter * sz);
  memset(results, 0, 2*iter*sz);
  if (results == NULL){
    perror("Process::vfork_destroy results allocation failure.");
    exit(1);
  }


  for (i=0;i<iter;i++){
    child_pid = vfork();
    if (child_pid == 0){
      /* This is the child process. */
      clock_gettime(CLOCK, &results[2*i]);
      _exit((int)1);
    }
    else if (child_pid == -1){
      /* This was an error */
      exit(1);
    }
    else{
      /* This is the parent process. */
      clock_gettime(CLOCK, &results[2*i+1]);
    }

  }

  discrete_elapsed_hr(overheads, results, &iter, "Process::vfork_destroy");

  free(results);
  free(overheads);
  return 0;
}
