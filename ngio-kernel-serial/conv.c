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
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include "utils.h"
#include "matrix_utils.h"

int main(int argc, char** argv){

  int *m, *n, *nz;
  int *row_csr_idx, *col_csr_idx;
  int *row_csc_idx, *col_csc_idx;
  float *A_csr, *B_csc, *C; // matrices
  double *tmp;

  char *filename = "matrix_in.txt";
  char line[64];

  int i,j,k;
  unsigned long rep = 0;
  int count=0, nz_count = 0;

  struct timespec start,end;

  m = malloc(sizeof(int));
  n = malloc(sizeof(int));
  nz = malloc(sizeof(int));

  get_matrix_size(filename, m, n, nz);
  printf("NZ = %d, M = %d, N = %d\n", *nz, *m, *n);


  row_csr_idx = malloc(*nz * sizeof(int));
  col_csr_idx = malloc(*nz * sizeof(int));
  A_csr = malloc(*nz * sizeof(float));
  tmp = malloc(*nz * sizeof(double));
  row_csc_idx = malloc(*nz * sizeof(int));
  col_csc_idx = malloc(*nz * sizeof(int));
  B_csc = malloc(*nz * sizeof(float));
  C = calloc((*m * *n), sizeof(float));

  if (!row_csr_idx || !col_csr_idx || !A_csr ||  !row_csc_idx || !col_csc_idx || !B_csc || !C){
    printf ("cannot allocate memory for %d, %d, %d sparse matrices and vector\n", *m, *n, *nz);
    exit(1);
  }
  else{
    printf("memory allocated\n");
  }

  clock_gettime(CLOCK, &start);
  mm_to_csr(filename, *m, *n, *nz, row_csr_idx, col_csr_idx, tmp);
  clock_gettime(CLOCK, &end);
  elapsed_time_hr(start, end, "MM to CSR conversion");
  /* A_csr = (float*) tmp; */

  free(m);
  free(n);
  free(nz);

  free(tmp);
  free(row_csr_idx);
  free(col_csr_idx);
  free(row_csc_idx);
  free(col_csc_idx);
  free(A_csr);
  free(B_csc);
  free(C);
  return 0;

}
