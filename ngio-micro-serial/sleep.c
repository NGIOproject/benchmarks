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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"
#include "level0.h"


/*
 * Measure the performance of sleep
 */
int sleep_sleep(unsigned int duration, unsigned int iter){

  struct timespec start, end;
  int i = 0;


  /*
   * In this benchmark, overheads are nil, we are measuring a single instruction
   */
  size_t sz = sizeof(struct timespec);
  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  memset(overheads, 0, 2*1000*sz);
  if (overheads == NULL){
    perror("Sleep::sleep overheads allocation failure.");
  }
  clock_gettime(CLOCK, &start);

  for (i=0;i<1000;i++){
    clock_gettime(CLOCK, &overheads[2*i]);
    clock_gettime(CLOCK, &overheads[2*i+1]);
  }

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * iter * sz);
  memset(results, 0, 2*iter*sz);
  if (results == NULL){
    perror("Sleep::sleep results allocation failure.");
  }


  for (i=0;i<iter;i++){
    clock_gettime(CLOCK, &results[2*i]);
    sleep(1*duration);
    clock_gettime(CLOCK, &results[2*i+1]);
  }

  discrete_elapsed_hr(overheads, results, &iter, "Sleep::sleep");
  free(results);
  free(overheads);

  return 0;
}

/*
 * Measure the performance of no-ops
 */
int sleep_nop(unsigned int duration, unsigned int iter){

  struct timespec start, end;
  int i = 0;
  int j = 0;
  int index, limit;
  limit = 1000*1000*100 * duration; // Chose this empirically to be ~1s on my laptop.


  /*
   * In this benchmark, overheads are nil, we are measuring a single instruction
   */
  size_t sz = sizeof(struct timespec);
  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  memset(overheads, 0, 2*1000*sz);
  if (overheads == NULL){
    perror("Sleep::no-op overheads allocation failure.");
  }
  clock_gettime(CLOCK, &start);

  for (i=0;i<1000;i++){
    clock_gettime(CLOCK, &overheads[2*i]);
    for (j=0;j<1;j++){
       __asm__ ("nop");
    }
    clock_gettime(CLOCK, &overheads[2*i+1]);
  }

  

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * iter * sz);
  memset(results, 0, 2*iter*sz);
  if (results == NULL){
    perror("Sleep::no-op results allocation failure.");
  }


  for (i=0;i<iter;i++){
    clock_gettime(CLOCK, &results[2*i]);
    for(index=0; index<limit; index++) {
      __asm__ ("nop");
    }
    clock_gettime(CLOCK, &results[2*i+1]);
  }

  discrete_elapsed_hr(overheads, results, &iter, "Sleep::no-op");
  free(results);
  free(overheads);

  return 0;


}

/*
 * Measure the performance of micro sleep
 */
int sleep_usleep(unsigned int duration, unsigned int iter){

  struct timespec start, end;
  int i = 0;


  /*
   * In this benchmark, overheads are nil, we are measuring a single instruction
   */
  size_t sz = sizeof(struct timespec);
  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  memset(overheads, 0, 2*1000*sz);
  if (overheads == NULL){
    perror("Sleep::usleep overheads allocation failure.");
  }
  clock_gettime(CLOCK, &start);

  for (i=0;i<1000;i++){
    clock_gettime(CLOCK, &overheads[2*i]);
    clock_gettime(CLOCK, &overheads[2*i+1]);
  }

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * iter * sz);
  memset(results, 0, 2*iter*sz);
  if (results == NULL){
    perror("Sleep::usleep results allocation failure.");
  }


  for (i=0;i<iter;i++){
    clock_gettime(CLOCK, &results[2*i]);
    usleep(1000*1000 * duration); // arg is in microseconds
    clock_gettime(CLOCK, &results[2*i+1]);
  }

  discrete_elapsed_hr(overheads, results, &iter, "Sleep::usleep");
  free(results);
  free(overheads);

  return 0;



}


/*
 * Measure the performance of nano sleep
 */
int sleep_nanosleep(unsigned int duration, unsigned int iter){

  struct timespec start, end;
  struct timespec slp;
  int i = 0;
  slp.tv_sec = (time_t)1 * duration;
  slp.tv_nsec = (long)0;

  /*
   * In this benchmark, overheads are nil, we are measuring a single instruction
   */
  size_t sz = sizeof(struct timespec);
  struct timespec* overheads = NULL;
  overheads = (struct timespec*)malloc(2 * 1000 * sz);
  memset(overheads, 0, 2*1000*sz);
  if (overheads == NULL){
    perror("Sleep::nanosleep overheads allocation failure.");
  }
  clock_gettime(CLOCK, &start);

  for (i=0;i<1000;i++){
    clock_gettime(CLOCK, &overheads[2*i]);
    clock_gettime(CLOCK, &overheads[2*i+1]);
  }

  struct timespec* results = NULL;
  results = (struct timespec*)malloc(2 * iter * sz);
  memset(results, 0, 2*iter*sz);
  if (results == NULL){
    perror("Sleep::nanosleep results allocation failure.");
  }


  for (i=0;i<iter;i++){
    clock_gettime(CLOCK, &results[2*i]);
    nanosleep(&slp, NULL);
    clock_gettime(CLOCK, &results[2*i+1]);
  }

  discrete_elapsed_hr(overheads, results, &iter, "Sleep::nanosleep");
  free(results);
  free(overheads);

  return 0;



}
