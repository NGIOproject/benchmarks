/* Copyright (c) 2017 The University of Edinburgh. */

/* 
* This software was developed as part of the                       
* EC H2020 funded project NEXTGenIO (Project ID: 671951)                 
* www.nextgenio.eu           
*/

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
#include <pthread.h>
#include <limits.h>

#include "level0.h"

/*
 *
 * Declare & Initialise the pthreads variables
 *
 */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_t callThd[2] = {0};
int p_flag = 0;
int flag_start = 0;
pthread_attr_t attr;

/*
 *
 * Level 0 benchmark driver - calls appropriate function
 * based on command line arguments.
 *
 */
#ifdef PMEM
void bench_level0(char *b, unsigned int s, unsigned int t, unsigned long r, char *o, char *dt, char *pmem_loc){
#else
void bench_level0(char *b, unsigned int s, unsigned int t, unsigned long r, char *o, char *dt){
#endif

  /* Basic operations */
  if(strcmp(b, "basic_op") == 0){

    if(strcmp(dt, "int") == 0)
      int_basic_op(o,r);

    else if(strcmp(dt, "float") == 0)
      float_basic_op(o,r);

    else if(strcmp(dt, "long") == 0)
      long_basic_op(o,r);

    else if(strcmp(dt, "double") == 0)
      double_basic_op(o,r);

    else fprintf(stderr, "ERROR: check you are using a valid data type...\n");
  }

  /* Memory reads and writes */
  else if(strcmp(b, "memory") == 0){

    if(strcmp(o, "calloc") == 0)
      mem_calloc(s);

#ifdef PMEM
    else if(strcmp(o, "pvmem_calloc") == 0)
      pvmem_calloc(s, pmem_loc);
#endif

    else if(strcmp(o, "read_ram") == 0)
      mem_read_ram(r);

#ifdef PMEM
    else if(strcmp(o, "read_vram") == 0)
      vmem_read_ram(r, pmem_loc);
#endif

    else if(strcmp(o, "write_contig") == 0)
      mem_write_contig(s);

#ifdef PMEM
    else if(strcmp(o, "write_vmem_contig") == 0)
      vmem_write_contig(s, pmem_loc);
#endif

    else if(strcmp(o, "write_strided") == 0)
      mem_write_strided(s, t);

#ifdef PMEM
    else if(strcmp(o, "write_vmem_strided") == 0)
      vmem_write_strided(s, t, pmem_loc);
#endif

    else if(strcmp(o, "write_random") == 0)
      mem_write_random(s);

#ifdef PMEM
    else if(strcmp(o, "write_vmem_random") == 0)
      vmem_write_random(s, pmem_loc);
#endif

    else if(strcmp(o, "read_contig") == 0)
      mem_read_contig(s);

#ifdef PMEM
    else if(strcmp(o, "read_vmem_contig") == 0)
      vmem_read_contig(s, pmem_loc);
#endif

    else if(strcmp(o, "read_lat") == 0)
      read_lat(s);

#ifdef PMEM
    else if(strcmp(o, "read_vmem_lat") == 0)
      vmem_read_lat(s, pmem_loc);
#endif

    else if(strcmp(o, "read_strided") == 0)
      mem_read_strided(s, t);

#ifdef PMEM
    else if(strcmp(o, "read_vmem_strided") == 0)
      vmem_read_strided(s, t, pmem_loc);
#endif

    else if(strcmp(o, "read_random") == 0)
      mem_read_random(s);

#ifdef PMEM
    else if(strcmp(o, "read_vmem_random") == 0)
      vmem_read_random(s, pmem_loc);
#endif

    else fprintf(stderr, "ERROR: check you are using a valid operation type...\n");
  }

  /* Function calls */
  else if(strcmp(b, "function") == 0){

    if(strcmp(o, "normal") == 0)
      function_calls(s);

    else if(strcmp(o, "recursive") == 0)
      function_calls_recursive(s);

    else fprintf(stderr, "ERROR: check you are using a valid operation type...\n");

  }

  /* IO operations */
  else if(strcmp(b, "io") == 0){

    if(strcmp(o, "mk_rm_dir") == 0)
      mk_rm_dir(s);

    else if(strcmp(o, "file_write") == 0)
      file_write(s);

#ifdef PMEM
    else if(strcmp(o, "file_pmem_write") == 0)
      pmem_file_write(s, pmem_loc);
#endif

    else if(strcmp(o, "file_read") == 0)
      file_read(s);

#ifdef PMEM
    else if(strcmp(o, "file_pmem_read") == 0)
      pmem_file_read(s, pmem_loc);
#endif

    else if(strcmp(o, "file_write_random") == 0)
      file_write_random(s);

#ifdef PMEM
    else if(strcmp(o, "file_pmem_write_random") == 0)
      pmem_file_write_random(s, pmem_loc);
#endif

    else if(strcmp(o, "file_read_random") == 0)
      file_read_random(s);

#ifdef PMEM
    else if(strcmp(o, "file_pmem_read_random") == 0)
      pmem_file_read_random(s, pmem_loc);
#endif

#ifndef __MACH__
    else if(strcmp(o, "file_read_direct") == 0)
      file_read_direct(s);

    else if(strcmp(o, "file_read_random_direct") == 0)
      file_read_random_direct(s);
#endif

    else fprintf(stderr, "ERROR: check you are using a valid operation type...\n");
  }

  /* IPC operations */
  else if (strcmp(b, "ipc") == 0){

    /* call into ipcmain with o as the argument */
    ipcmain(o, r, s);

  }

  /* Network operations */
  else if (strcmp(b, "net") == 0){

    /* call into netmain with o as the argument */
    netmain(o, r);

  }

  /* Branches/jumps */
  else if (strcmp(b, "branch") == 0){

    if(strcmp(o, "all_true") == 0)
      all_true(r);

    else if(strcmp(o, "all_false") == 0)
      all_false(r);

    else if(strcmp(o, "true_false") == 0)
      true_false(r);

    else if(strcmp(o, "t2_f2") == 0)
      t2_f2(r);

    else if(strcmp(o, "t4_f4") == 0)
      t2_f2(r);

    else if(strcmp(o, "t8_f8") == 0)
      t2_f2(r);

    else if(strcmp(o, "t_f_random") == 0)
      t_f_random(r);

    else if(strcmp(o, "switch") == 0)
      switch_jump(r);

    else if(strcmp(o, "goto") == 0)
      goto_jump(r);

    else fprintf(stderr, "ERROR: check you are using a valid operation type...\n");

  }

  /* Sleep */
  else if(strcmp(b, "sleep") == 0){

	/* if r is not specified, set to 1000 - ULONG_MAX reps is too high for this benchmark */
    if (r == ULONG_MAX) r = 1000;

    /* if 'o' is not specified, go for normal sleep */
    if (strcmp(o, "sleep") == 0 || strcmp(o, "+") == 0)
      sleep_sleep(s, r);

    else if(strcmp(o, "nop") == 0)
      sleep_nop(s, r);

    else if(strcmp(o, "usleep") == 0)
      sleep_usleep(s, r);

    else if(strcmp(o, "nanosleep") == 0)
      sleep_nanosleep(s, r);

    else fprintf(stderr, "ERROR: check you are requesting a valid sleep type...\n");

  }

  /* Processes and threads */
  else if(strcmp(b, "process") == 0){

    /* if r is not specified, set to 1000 - ULONG_MAX reps is too high for this benchmark */
    if (r == ULONG_MAX) r = 1000;
    
    if (strcmp(o, "fork_create") == 0)
      process_fork_create(r);

    else if (strcmp(o, "fork_destroy") == 0)
      process_fork_destroy(r);

    else if (strcmp(o, "vfork_create") == 0)
      process_vfork_create(r);

    else if (strcmp(o, "vfork_destroy") == 0)
      process_vfork_destroy(r);

    else if (strcmp(o, "pthread_create") == 0)
      process_pthread_create(r);

    else if (strcmp(o, "pthread_destroy") == 0)
      process_pthread_destroy(r);

    else fprintf(stderr, "ERROR: check you are requesting a valid process type...\n");

  }

  /* everything that has not been implemented */
  else
    printf("Not implemented yet.\n");

}
