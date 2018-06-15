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
  int N = 0, i = 0, j = 0, k = 0, rep = 0;
  int n_files = 0;

  char titlebuffer[500] = "";
  int fd = 0;

  /* data size */
  N = pow(1024,2);
  size = sizeof(char);
  data = (char *) malloc(N*size);

  if ( argc != 34)  {
    fprintf(stderr, "ERROR: incorrect usage (repetitions number_of_files path).\n");
    return -10;
  }

  rep = atoi(argv[1]);
  n_files = atoi(argv[2]);
  path = argv[3];

  sprintf(path+strlen(path), "/testfile");

  if (!data) {
    fprintf(stderr, "ERROR: out of memory\n");
    return -1;
  }
  memset(data, '0', N * size);

  sprintf(titlebuffer, "Reading %d files of %lu bytes", n_files, (long unsigned int) (N*size*rep));
    
  /* time the read test */
  clock_gettime(CLOCK_MONOTONIC, &start);
    
  /* loop over number of files */
  for(i=0;i<n_files;i++){

    strcpy(name,path);
    sprintf(name+strlen(name), "_%d", i);

    fd = open(name, O_RDONLY);
    if (fd < 0) {
      fprintf(stderr, "ERROR: unable to open test file for reading\n");
      return 1;
    }
    
    /* loop over 1MB chunks */
    for(j=0; j<rep; j++){
      read(fd, data, N*size);
    }
    
    fsync(fd);
    close(fd);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  elapsed_time_hr(start, end, titlebuffer);

  data = (char *) realloc(data, rep * N * size);
  for ( i = 0 ; i < n_files ; i++ ) {
    memset(data, '0', rep * N * size);
    sprintf(name, "testfile_%d", i);

    fd = open(name, O_RDONLY);
    if (fd < 0) {
      fprintf(stderr, "ERROR: unable to open test file for reading\n");
      return 1;
    }

    read(fd, data, rep * N * size);
  
    for ( j = 0 ; j < rep ; j++ )  {
      if ( strncmp("1", &data[j * N * size], 1) != 0 ||
           strncmp("1", &data[j * N * size + (N * size - 1)], 1) != 0 )  {
        fprintf(stderr, "ERROR: invalid value at %d (found %c).\n", j, data[j]);
        free(data);
        return -10;
      }
    
      for ( k = 1 ; k < ( N * size - 1 ) ; k++ )  {
        if ( strncmp("6", &data[(j * N * size + k)], 1) != 0  )  {
          fprintf(stderr, "ERROR: invalid value at %d (found %c).\n", j, data[j]);
          free(data);
          return -11;
        }
      }
    }
    close(fd);
  }

  free(data);
  fflush(stdout);
  return 0; 
}

