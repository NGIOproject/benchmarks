/* Copyright (c) 2017 The University of Edinburgh. */

/* 
* This software was developed as part of the                       
* EC H2020 funded project NEXTGenIO (Project ID: 671951)                 
* www.nextgenio.eu           
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <libpmem.h>

// PMEM_IS_PMEM_FORCE=1 ./producer 10 2  // 2 files 10 MBs each
#define BUF_LEN 4096

#include "utils.h"

int main(int argc, char **argv){
  struct timespec start, end;

  char *path;
  char name[100] = "";
  int size = 0;
  char *data = NULL, *pmemaddr = NULL;
  int N = 0, i = 0, j = 0, rep = 0, is_pmem = 0;
  int n_files = 0;
  size_t mapped_len;

  char titlebuffer[500] = "";

  /* allocate and initialise data */
  N = pow(1024,2);
  size = sizeof(char);
  data = (char*) malloc(N * size);

  if ( argc != 4 )  {
    fprintf(stderr, "ERROR: incorrect usage (repetitions number_of_files path).\n");
    return -10;
  }

  rep = atoi(argv[1]);
  n_files = atoi(argv[2]);
  path = argv[3];

  sprintf(path+strlen(path), "/testfile");

  if (!data) {
    fprintf(stderr, "ERROR: out of memory in file_write\n");
    return -1;
  }
  memset(data, '6', N * size);
  memset(&data[0], '1', size);
  memset(&data[(N * size) - 1], '1', size);
  sprintf(titlebuffer, "Writing %d files of %lu bytes to directory %s", n_files, (unsigned long) (N*size*rep), path);

  /* do actual write test */
  clock_gettime(CLOCK_MONOTONIC, &start);

 
  /* loop over number of files */
  for(i=0;i<n_files;i++){

    strcpy(name,path);
    sprintf(name+strlen(name), "_%d", i);

    if ((pmemaddr = pmem_map_file(name, rep * N * size,
                             PMEM_FILE_CREATE|PMEM_FILE_EXCL,
                             0666, &mapped_len, &is_pmem)) == NULL) {
                                  perror("pmem_map_file");
                                  fprintf(stderr, "Failed to pmem_map_file for filename:%s.\n", name);
                                  exit(-100);
                                }

    if(is_pmem){

      /* loop over chunks */
      for(j=0; j<rep; j++){
        pmem_memcpy_nodrain(pmemaddr, data, N * size);
        pmemaddr += N * size;
      }

      pmem_drain();
      pmemaddr -= rep * N * size;
 
    }else{

       printf("Not pmem\n");

    }

    pmem_unmap(pmemaddr, mapped_len);

  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  elapsed_time_hr(start, end, titlebuffer);
    
  free(data);
  fflush(stdout);
  return 0; 
}

