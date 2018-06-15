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

#include "utils.h"

int main(int argc, char **argv){
  struct timespec start, end;

  char *path;
  char name[100] = "";
  int size = 0;
  char *data = NULL;
  int N = 0, i = 0, j = 0, rep = 0;
  int n_files = 0;

  char titlebuffer[500] = "";
  int fd = 0;

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
  sprintf(titlebuffer, "Writing %d files of %lu bytes", n_files, (unsigned long) (N*size*rep));
    
  /* do actual write test */
  clock_gettime(CLOCK_MONOTONIC, &start);

  /* loop over number of files */
  for(i=0;i<n_files;i++){

    strcpy(name,path);
    sprintf(name+strlen(name), "_%d", i);
    
    fd = open(name, O_CREAT|O_WRONLY|O_TRUNC|O_APPEND, 0644);
    if (fd < 0) {
      fprintf(stderr, "ERROR: unable to open testfile for writing\n");
      return 1;
    }
    
    /* loop over chunks */
    for(j=0; j<rep; j++){
      write(fd, data, N*size);
    }

    fsync(fd);
    close(fd);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  elapsed_time_hr(start, end, titlebuffer);
    
  free(data);
  fflush(stdout);
  return 0; 
}

