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

#include <pthread.h>
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern pthread_t callThd[];
extern int p_flag;
extern int flag_start;
extern pthread_attr_t attr;

struct ipc_thread_info {
  unsigned long iteration;
  unsigned int payload_size;
};

#ifdef PMEM
void bench_level0(char *, unsigned int, unsigned int, unsigned long, char *, char *, char *);
#else
void bench_level0(char *, unsigned int, unsigned int, unsigned long, char *, char *);
#endif

/* Basic op */
int int_basic_op(char *, unsigned long);
int float_basic_op(char *, unsigned long);
int double_basic_op(char *, unsigned long);
int long_basic_op(char *, unsigned long);

/* Memory */
int mem_calloc(unsigned int);
#ifdef PMEM
int pvmem_calloc(unsigned int, char *);
#endif
int mem_read_ram(unsigned long);
#ifdef PMEM
int vmem_read_ram(unsigned long, char*);
#endif
int mem_write_contig(unsigned int);
#ifdef PMEM
int vmem_write_contig(unsigned int, char*);
#endif
int mem_write_strided(unsigned int, unsigned int);
#ifdef PMEM
int vmem_write_strided(unsigned int, unsigned int, char*);
#endif
int mem_write_random(unsigned int);
#ifdef PMEM
int vmem_write_random(unsigned int, char*);
#endif
int mem_read_contig(unsigned int);
#ifdef PMEM
int vmem_read_contig(unsigned int, char*);
#endif
int mem_read_strided(unsigned int, unsigned int);
#ifdef PMEM
int vmem_read_strided(unsigned int, unsigned int, char*);
#endif
int mem_read_random(unsigned int);
#ifdef PMEM
int vmem_read_random(unsigned int, char *nvmelocation);
#endif
int read_lat(unsigned int);
#ifdef PMEM
int vmem_read_lat(unsigned int, char*);
#endif
/* Function calls */
int function_calls(unsigned int);
int function_calls_recursive(unsigned int);

/* IO operations */
int mk_rm_dir(unsigned int);
int file_write(unsigned int);
#ifdef PMEM
int pmem_file_write(unsigned int, char*);
#endif
int file_write_random(unsigned int);
#ifdef PMEM
int pmem_file_write_random(unsigned int, char*);
#endif
int file_read(unsigned int);
#ifdef PMEM
int pmem_file_read(unsigned int, char*);
#endif
#ifndef __MACH__
int file_read_direct(unsigned int);
#endif
int file_read_random(unsigned int);
#ifdef PMEM
int pmem_file_read_random(unsigned int, char*);
#endif
#ifndef __MACH__
int file_read_random_direct(unsigned int);
#endif

/* IPC operations */
int ipcmain(char *, unsigned long, unsigned int);
void* shm_server(void*);
void* shm_client(void*);
void* fifo_server(void*);
void* socket_server(void*);
void* shm_client(void*);
void* fifo_client(void*);
void* socket_client(void*);

/* Network operations */
int netmain(char *, unsigned long);
void* tcp_server(void*);
void* tcp_client(void*);
void* udp_server(void*);
void* udp_client(void*);

/* Branches/jumps */
int all_true(unsigned long);
int all_false(unsigned long);
int true_false(unsigned long);
int t2_f2(unsigned long);
int t2_f2(unsigned long);
int t2_f2(unsigned long);
int t_f_random(unsigned long);
int switch_jump(unsigned long);
int goto_jump(unsigned long);

/* Sleep */
int sleep_sleep(unsigned int, unsigned int);
int sleep_nop(unsigned int, unsigned int);
int sleep_usleep(unsigned int, unsigned int);
int sleep_nanosleep(unsigned int, unsigned int);

/* Processes and threads*/
int process_pthread_create(unsigned int);
int process_pthread_destroy(unsigned int);
int process_fork_create(unsigned int);
int process_fork_destroy(unsigned int);
int process_vfork_create(unsigned int);
int process_vfork_destroy(unsigned int);
