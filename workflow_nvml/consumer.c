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

// PMEM_IS_PMEM_FORCE=1 ./consumer 10 2  // read 2 files 10 MBs each
#define BUF_LEN 4096

#include "utils.h"

int main(int argc, char **argv){
  
  struct timespec start, end;

  char name[100] = "";
  int size = 0;
  char *data = NULL, *pmemaddr = NULL;
  int N = 0, i = 0, j = 0, k = 0, rep = 0, is_pmem = 0;
  int n_files = 0;
  size_t mapped_len;

  char titlebuffer[500] = "";

  /* data size */
  N = pow(1024,2);
  size = sizeof(char);
  data = (char *) malloc(N*size);

  if ( argc != 3 )  {
    fprintf(stderr, "ERROR: incorrect usage (repetitions number_of_files).\n");
    return -10;
  }

  rep = atoi(argv[1]);
  n_files = atoi(argv[2]);

  if (!data) {
    fprintf(stderr, "ERROR: out of memory\n");
    return -1;
  }
  memset(data, '0', N * size);

  sprintf(titlebuffer, "Reading %d files of %lu bytes", n_files, (unsigned long) (N*size*rep));
  int local_reps =  (rep * N * size) / BUF_LEN;
  int remainder = (rep * N * size) % BUF_LEN;
  printf("local_reps:%d, remainder:%d.\n", local_reps, remainder);

  /* time the read test */
  clock_gettime(CLOCK_MONOTONIC, &start);
    
  /* loop over number of files */
  for(i=0;i<n_files;i++){

    sprintf(name, "testfile_%d", i);
    
    if ((pmemaddr = pmem_map_file(name, 0,
                             0,
                             0666, &mapped_len, &is_pmem)) == NULL) {
                                  perror("pmem_map_file");
                                  fprintf(stderr, "Failed to pmem_map_file for filename:%s.\n", name);
                                  exit(-100);
                             }

    /* loop over 1MB chunks */
//    for(j=0; j<rep; j++){
//      memcpy(data, pmemaddr, N * size);
//      pmemaddr += N * size;
//    }
    for ( j = 0 ; j < local_reps ; j++ )  {
      memcpy(data, pmemaddr, BUF_LEN);
      pmemaddr += BUF_LEN;
    }
   
//    pmemaddr -= rep * N * size;
    pmemaddr -= local_reps * BUF_LEN;
    pmem_unmap(pmemaddr, mapped_len);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  elapsed_time_hr(start, end, titlebuffer);

  data = (char *) realloc(data, rep * N * size);
  for ( i = 0 ; i < n_files ; i++ ) {
    memset(data, '0', rep * N * size);
    sprintf(name, "testfile_%d", i);

    if ((pmemaddr = pmem_map_file(name, 0,
                             0,
                             0666, &mapped_len, &is_pmem)) == NULL) {
                                  perror("pmem_map_file");
                                  fprintf(stderr, "Failed to pmem_map_file for filename:%s.\n", name);
                                  exit(-100);
                             }

    memcpy(data, pmemaddr, rep * N * size);
  
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
    pmem_unmap(pmemaddr, mapped_len);
  }

  free(data);
  fflush(stdout);
  return 0; 
}

