#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<libpmem.h>
#include<omp.h>
#include"utils.h"
#define ARRAY_SIZE 100000000
#define MB 1048576
#define REPEATS 10

void copy(double *, double *, long int);
void scale(double *, double *, double, long int);
void add(double *, double *, double *, long int);
void triadd(double *, double *, double *, double, long int);
void initialise(double *, double *, double *, long int);
int main(int argc, char *argv[]){

  struct timespec start, end;
  
  char *path;
  char title[100] = "";
  double *a, *b, *c;
  char *pmemaddr = NULL;
  long int array_size, bytes;
  int repeats;
  int i;
  int array_element_size;
  int is_pmem;
  size_t mapped_len;
  int num_threads;
 
  if(argc != 4){
    array_size = ARRAY_SIZE;
    repeats = REPEATS;
    path = "";
  }else{
    array_size = atoi(argv[1]);
    repeats = atoi(argv[2]);
    path = argv[3];
  }

  a = malloc(sizeof(double)*array_size);
  b = malloc(sizeof(double)*array_size);
  c = malloc(sizeof(double)*array_size);

  array_element_size = sizeof(a[0]);

  printf("Using an array of %ld doubles (%ld MB) for experiments\n",array_size,array_size*array_element_size/MB);
#pragma omp parallel shared(num_threads)
  {
    num_threads = omp_get_num_threads();
  }
  printf("Running on %d threads\n", num_threads);

  printf("Memory test\n");

  initialise(a,b,c,array_size);

  clock_gettime(CLOCK_MONOTONIC, &start);
  for(i=0; i<repeats; i++){
    copy(a,b,array_size);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  strcpy(title, "Copy");
  elapsed_time_bw_hr(start, end, repeats, (2*array_size*array_element_size)/MB, title);

  clock_gettime(CLOCK_MONOTONIC, &start);
  for(i=0; i<repeats; i++){
    scale(a,c,2.4,array_size);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  strcpy(title, "Scale");
  elapsed_time_bw_hr(start, end, repeats, (2*array_size*array_element_size)/MB, title);

  clock_gettime(CLOCK_MONOTONIC, &start);
  for(i=0; i<repeats; i++){
    add(b,a,c,array_size);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  strcpy(title, "Add");
  elapsed_time_bw_hr(start, end, repeats, (3*array_size*array_element_size)/MB, title);

  clock_gettime(CLOCK_MONOTONIC, &start);
  for(i=0; i<repeats; i++){
    triadd(a,b,c,2.4,array_size);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  strcpy(title, "Triadd");
  elapsed_time_bw_hr(start, end, repeats, (3*array_size*array_element_size)/MB, title);

  free(a);
  free(b);
  free(c);

  printf("PMem test\n");

  sprintf(path+strlen(path), "pstream_test_file");

  if ((pmemaddr = pmem_map_file(path, array_size*array_element_size*3,
				PMEM_FILE_CREATE|PMEM_FILE_EXCL,
				0666, &mapped_len, &is_pmem)) == NULL) {
    perror("pmem_map_file");
    fprintf(stderr, "Failed to pmem_map_file for filename:%s.\n", path);
    exit(-100);
  }

  printf("Using file %s for pmem\n",path);

  a = pmemaddr;
  b = pmemaddr + array_size*array_element_size;
  c = pmemaddr + array_size*array_element_size*2;

  initialise(a,b,c,array_size);

  clock_gettime(CLOCK_MONOTONIC, &start);
  for(i=0; i<repeats; i++){
    copy(a,b,array_size);
    pmem_persist(pmemaddr, array_size*array_element_size);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  strcpy(title, "Copy");
  elapsed_time_bw_hr(start, end, repeats, (2*array_size*array_element_size)/MB, title);

  clock_gettime(CLOCK_MONOTONIC, &start);
  for(i=0; i<repeats; i++){
    scale(a,c,2.4,array_size);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  strcpy(title, "Scale");
  elapsed_time_bw_hr(start, end, repeats, (2*array_size*array_element_size)/MB, title);

  clock_gettime(CLOCK_MONOTONIC, &start);
  for(i=0; i<repeats; i++){
    add(b,a,c,array_size);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  strcpy(title, "Add");
  elapsed_time_bw_hr(start, end, repeats, (3*array_size*array_element_size)/MB, title);

  clock_gettime(CLOCK_MONOTONIC, &start);
  for(i=0; i<repeats; i++){
    triadd(a,b,c,2.4,array_size);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  strcpy(title, "Triadd");
  elapsed_time_bw_hr(start, end, repeats, (3*array_size*array_element_size)/MB, title);

  pmem_persist(pmemaddr, mapped_len);

  pmem_unmap(pmemaddr, mapped_len);

  return 0;

}

void initialise(double *a, double *b, double *c, long int array_size){

 int j;
  
#pragma omp parallel for
  for (j=0; j<array_size; j++){
    a[j] = 0.0;
    b[j] = 2.0;
    c[j] = 1.0;
  }

  return;

}

void copy(double *a, double *b, long int array_size){
  
  int j;
  
#pragma omp parallel for
  for (j=0; j<array_size; j++){
    b[j] = a[j];
  }
  return;
}
 
void scale(double *a, double *b, double scalar, long int array_size){
  
  int j;
  
#pragma omp parallel for
  for (j=0; j<array_size; j++){
    b[j] = a[j]*scalar;
  }
  
  return;
  
}

void add(double *a, double *b, double *c, long int array_size){
  
  int j;
  
#pragma omp parallel for
  for (j=0; j<array_size; j++){
    c[j] = a[j]+b[j];
  }
  
  return;
  
}

void triadd(double *a, double *b, double *c, double scalar, long int array_size){
  
  int j;
  
#pragma omp parallel for
  for (j=0; j<array_size; j++){
    c[j] = a[j]+b[j]*scalar;
  }
  
  return;
  
}
