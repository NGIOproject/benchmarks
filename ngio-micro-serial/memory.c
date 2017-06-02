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
#include <math.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "level0.h"
#include "utils.h"

#ifdef PMEM
#include<libvmem.h>
#endif

/* callocate 'size' amount of memory (in MB), then free it. */
int mem_calloc(unsigned int size){
    
  struct timespec start, end;

  /* work out requested size in Bytes */
  int nbytes = size * 1048576;
  int nelements = nbytes / sizeof(int);
  
  clock_gettime(CLOCK, &start);

  
  /* allocate array of nbytes */
  int *array = (int *)calloc(nelements, sizeof(int));
  clock_gettime(CLOCK, &end);
  
  if(array == NULL){
    printf("Out Of Memory: could not allocate space for the array.\n");
    return 0;
  }
  
  elapsed_time_hr(start, end, "Allocate memory");
  
  printf("Finished callocating %d MB memory.\n\n", size);
  
  free(array); // Remember to free !
    
  return 0;
  
}


#ifdef PMEM
/* callocate 'size' amount of memory (in MB), then free it. */
int pvmem_calloc(unsigned int size, char *nvmelocation){

  struct timespec start, end;

  /* work out requested size in Bytes */
  int nbytes = size * 1048576;
  int nelements = nbytes / sizeof(int);

  VMEM *vmp;
  int *array;

  clock_gettime(CLOCK, &start);

  /* create minimum size pool of memory */
  if ((vmp = vmem_create(nvmelocation,
			 nbytes*2)) == NULL) {
    perror("vmem_create");
    exit(1);
  }

  if((array = vmem_calloc(vmp, nelements, sizeof(int))) == NULL){
                perror("vmem_calloc");
                exit(1);
  }

  clock_gettime(CLOCK, &end);

  if(array == NULL){
    printf("Out Of Memory: could not allocate space for the array.\n");
    return 0;
  }

  elapsed_time_hr(start, end, "Allocate memory");

  printf("Finished callocating %d MB memory.\n\n", size);

  //free
  vmem_free(vmp, array);

  vmem_delete(vmp);

  return 0;

}
#endif

/* measure main memory (RAM) reads */
int mem_read_ram(unsigned long reps){

  struct timespec start, end;

  int i, a;
  volatile int b, c;

  srand((int)time(NULL));
  b = rand();
  c = rand();

  clock_gettime(CLOCK, &start);


  for(i=0; i<reps; i++){
    a = b + c;
  }

  clock_gettime(CLOCK, &end);
  elapsed_time_hr(start, end, "RAM reads");
  
  printf("Finished %lu RAM reads.\n\n", (2 * reps));
    
  return 0;
}


#ifdef PMEM
/* measure pmem.io vmem reads */
int vmem_read_ram(unsigned long reps, char *nvmelocation){

  struct timespec start, end;

  int i, a;

  VMEM *vmp;
  volatile int *b,*c;

  clock_gettime(CLOCK, &start);

  /* create minimum size pool of memory */
  if ((vmp = vmem_create(nvmelocation,
                         VMEM_MIN_POOL)) == NULL) {
    perror("vmem_create");
    exit(1);
  }

  if((b = vmem_malloc(vmp, sizeof(int))) == NULL){
                perror("vmem_malloc");
                exit(1);
  }

  if((c = vmem_malloc(vmp, sizeof(int))) == NULL){
                perror("vmem_malloc");
                exit(1);
  }

  srand((int)time(NULL));
  *b = rand();
  *c = rand();

  clock_gettime(CLOCK, &start);


  for(i=0; i<reps; i++){
    a = *b + *c;
  }

  clock_gettime(CLOCK, &end);
  elapsed_time_hr(start, end, "RAM reads");
  
  printf("Finished %lu RAM reads.\n\n", (2 * reps));
    

  //free
  vmem_free(vmp, b);
  vmem_free(vmp, c);

  vmem_delete(vmp);

  return 0;
}
#endif

/* allocate 'size' amount of memory (in MB)        */
/* write a random value to all the array elements  */
/* in a contiguous manner and then free the memory. */
int mem_write_contig(unsigned int size){
  
  int i;
  /* work out number of bytes from size in MB */
  int nbytes = size * 1048576; 
  /* work out number of ints that fit into nbytes */
  int nelements = nbytes / sizeof(int); 

  double oh, rt;

  struct timespec start, end;
  
  srand((int)time(NULL));
  
  int data = rand();

  /* allocate array of nbytes */
  int *array = (int *)malloc(nbytes);
  
  if(array == NULL){
    printf("Out Of Memory: could not allocate space for the array.\n");
    return 0;
  }
  
  /* loop overhead */
  clock_gettime(CLOCK, &start);


  for(i = 0; i < nelements; i++){
    __asm__ ("nop");
  }
  
  clock_gettime(CLOCK, &end);
  oh = elapsed_time_hr(start, end, "Loop overhead for contiguous write");


  clock_gettime(CLOCK, &start);


  /* write data to array in contiguous manner - loop over i */
  for(i = 0; i < nelements; i++){
    array[i] = data;
  }
  
  clock_gettime(CLOCK, &end);
  rt = elapsed_time_hr(start, end, "Write contiguously");
  
  printf("Make sure compiler keeps result: array[0] = %d\n", array[0]);

  printf("Runtime: %f\n", (rt-oh));  
  
  printf("Finished allocating %d MB memory for %d ints and "
	 "filling the array in a contiguous manner.\n\n", size, nelements);
  
  free(array); // Remember to free !
  /* free(indices); */

  return 0;
  
}

/* allocate 'size' amount of pmem.io vmem (in MB)        */
/* write a random value to all the array elements  */
/* in a contiguous manner and then free the memory. */
#ifdef PMEM
int vmem_write_contig(unsigned int size, char *nvmelocation){

  int i;
  /* work out number of bytes from size in MB */
  int nbytes = size * 1048576;
  /* work out number of ints that fit into nbytes */
  int nelements = nbytes / sizeof(int);

  double oh, rt;

  struct timespec start, end;

  VMEM *vmp;

  /* create minimum size pool of memory */
  if ((vmp = vmem_create(nvmelocation,
                         nbytes+nbytes/2)) == NULL) {
    perror("vmem_create");
    exit(1);
  }


  srand((int)time(NULL));

  int data = rand();

  int *array;

  /* allocate array of nbytes */
  if((array = (int *)vmem_malloc(vmp, nbytes)) == NULL){
                perror("vmem_malloc");
                exit(1);
  }

  if(array == NULL){
    printf("Out Of Memory: could not allocate space for the array.\n");
    return 0;
  }

  /* loop overhead */
  clock_gettime(CLOCK, &start);


  for(i = 0; i < nelements; i++){
    __asm__ ("nop");
  }

  clock_gettime(CLOCK, &end);
  oh = elapsed_time_hr(start, end, "Loop overhead for contiguous write");


  clock_gettime(CLOCK, &start);


  /* write data to array in contiguous manner - loop over i */
  for(i = 0; i < nelements; i++){
    array[i] = data;
  }

  clock_gettime(CLOCK, &end);
  rt = elapsed_time_hr(start, end, "Write contiguously");

  printf("Make sure compiler keeps result: array[0] = %d\n", array[0]);

  printf("Runtime: %f\n", (rt-oh));

  printf("Finished allocating %d MB memory for %d ints and "
         "filling the array in a contiguous manner.\n\n", size, nelements);


  vmem_free(vmp, array);

  vmem_delete(vmp);

  return 0;

}
#endif


/* allocate 'size' MB of memory, write a random    */
/* value to all the  array elements in a strided   */
/* manner and then free the memory.                */
int mem_write_strided(unsigned int size, unsigned int stride){
  
  int i;
  int j, s, n = 0;
  unsigned int nbytes = size * 1048576;
  unsigned int strbytes = stride * 1024;

  /* calculate total number of elements to be allocated/written */
  unsigned int nelements = nbytes / sizeof(int);
  /* convert stride size into ints */
  unsigned int str = strbytes / sizeof(int);

  double oh, rt;

  struct timespec start, end;

  srand((int)time(NULL));
  
  int data = rand();
  
  /* perform the strided write for all stride values */
  /* from 2 up to str                                */
  for(s=2; s<=str; s=s*2){

    /* allocate 'size' MB of memory              */
    /* get a fresh allocation for each iteration */
    int *array = (int *)malloc(nbytes);
    
    if(array == NULL){
      printf("Out Of Memory: could not allocate space for the array.\n");
      return 0;
    }

    printf("Memory size in ints: %d, stride in ints: %d\n", nelements, s);
    
    /* measure the overhead */
    clock_gettime(CLOCK, &start);

    
    for(i = 0; i < s; i++){
      for(j = 0; j < nelements; j=j+s){
        if(i+j < nelements) {
          n = i+j;
          }
        if(n == nelements-1) break;
        n++;
      }
      if(n == nelements-1) break;
    }

    clock_gettime(CLOCK, &end);
    oh = elapsed_time_hr(start, end, "Overhead for strided write");

    /* make sure the compiler executes the loop that calculates n */
    printf("n = %d\n", n);

    n = 0;
    
    clock_gettime(CLOCK, &start);

    
    /* write data to array in strided manner */
    for(i = 0; i < s; i++){
      for(j = 0; j < nelements; j=j+s){
        if(i+j < nelements) {
          n = i+j;
          array[n]=data;
        }
        if(n == nelements-1) break;
        n++;
      }
      if(n == nelements-1) break;
    }
    
    clock_gettime(CLOCK, &end);
    rt = elapsed_time_hr(start, end, "Strided write");
    
    /* make sure the compiler executes the loop that calculates n */
    printf("n = %d, array[0] = %d\n", n, array[0]);

    printf("Runtime: %f\n", (rt-oh));  
  
    free(array); // Remember to free !
  
  }

  printf("Finished allocating %d MB memory and "
	 "filling the array in a strided manner "
	 "up to a stride size of %d KB.\n\n", size, stride);
      
  return 0;
  
}

#ifdef PMEM
/* allocate 'size' MB of memory using pmem.io vmem, write a random    */
/* value to all the  array elements in a strided   */
/* manner and then free the memory.                */
int vmem_write_strided(unsigned int size, unsigned int stride, char *nvmelocation){
  
  int i;
  int j, s, n = 0;
  unsigned int nbytes = size * 1048576;
  unsigned int strbytes = stride * 1024;

  /* calculate total number of elements to be allocated/written */
  unsigned int nelements = nbytes / sizeof(int);
  /* convert stride size into ints */
  unsigned int str = strbytes / sizeof(int);

  VMEM *vmp;
  int *array;

  double oh, rt;

  struct timespec start, end;

  srand((int)time(NULL));
  
  int data = rand();
  

  /* create minimum size pool of memory */
  if ((vmp = vmem_create(nvmelocation,
                         nbytes*2)) == NULL) {
    perror("vmem_create");
    exit(1);
  }

  /* perform the strided write for all stride values */
  /* from 2 up to str                                */
  for(s=2; s<=str; s=s*2){

    /* allocate 'size' MB of memory              */
    /* get a fresh allocation for each iteration */
    if((array = (int *)vmem_malloc(vmp, nbytes)) == NULL){
      perror("vmem_malloc");
      exit(1);
    }

    
    if(array == NULL){
      printf("Out Of Memory: could not allocate space for the array.\n");
      return 0;
    }

    printf("Memory size in ints: %d, stride in ints: %d\n", nelements, s);
    
    /* measure the overhead */
    clock_gettime(CLOCK, &start);

    
    for(i = 0; i < s; i++){
      for(j = 0; j < nelements; j=j+s){
        if(i+j < nelements) {
          n = i+j;
          }
        if(n == nelements-1) break;
        n++;
      }
      if(n == nelements-1) break;
    }

    clock_gettime(CLOCK, &end);
    oh = elapsed_time_hr(start, end, "Overhead for strided write");

    /* make sure the compiler executes the loop that calculates n */
    printf("n = %d\n", n);

    n = 0;
    
    clock_gettime(CLOCK, &start);

    
    /* write data to array in strided manner */
    for(i = 0; i < s; i++){
      for(j = 0; j < nelements; j=j+s){
        if(i+j < nelements) {
          n = i+j;
          array[n]=data;
        }
        if(n == nelements-1) break;
        n++;
      }
      if(n == nelements-1) break;
    }
    
    clock_gettime(CLOCK, &end);
    rt = elapsed_time_hr(start, end, "Strided write");
    
    /* make sure the compiler executes the loop that calculates n */
    printf("n = %d, array[0] = %d\n", n, array[0]);

    printf("Runtime: %f\n", (rt-oh));  
  
    vmem_free(vmp, array); // Remember to free !
  
  }

  vmem_delete(vmp);

  printf("Finished allocating %d MB memory and "
	 "filling the array in a strided manner "
	 "up to a stride size of %d KB.\n\n", size, stride);
      
  return 0;
  
}
#endif

/* allocate memory for 'size' number of elements   */
/* of type 'dt', write a random value to all the   */
/* array elements using random access and then     */
/* free the memory.                                */
int mem_write_random(unsigned int size){
  
  int i, n;
  unsigned int t;
  /* work out number of bytes from size in MB */
  int nbytes = size * 1048576;
  /* work out number of ints that fit into nbytes */
  int nelements = nbytes / sizeof(int);

  double oh, rt = 0.0;

  struct timespec start, end;
  
  t = (int)time(NULL);

  /* allocate 'size' MB of memory */
  int *array = (int *)malloc(nbytes);
  
  if(array == NULL){
    printf("Out Of Memory: could not allocate space for the array.\n");
    return 0;
  }

  srand(t);

  clock_gettime(CLOCK, &start);


  /* measure overheads */
  for(i=0; i<nelements; i++){
    n = rand() % (nelements);
  }

  clock_gettime(CLOCK, &end);
  oh = elapsed_time_hr(start, end, "Overhead for random write");

  printf("Keep result: n = %d\n", n);  
  
  srand(t);
  int data = rand();

  clock_gettime(CLOCK, &start);
  
  
  /* write data to array in random manner */
  for(i = 0; i < nelements; i++){
    n = rand() % (nelements);
    array[n] = data;
  }
    
  clock_gettime(CLOCK, &end);
  rt = elapsed_time_hr(start, end, "Write random");

  printf("Runtime: %f\n", (rt-oh));
  
  printf("Finished allocating %d MB memory (%d ints) and "
	 "filling the array in a random manner.\n\n", size, nelements);
  
  free(array); // Remember to free !

  return 0;
  
}

#ifdef PMEM
/* allocate memory (using pmem.io vmem) for 'size' number of elements   */
/* of type 'dt', write a random value to all the   */
/* array elements using random access and then     */
/* free the memory.                                */
int vmem_write_random(unsigned int size, char *nvmelocation){
  
  int i, n;
  unsigned int t;
  /* work out number of bytes from size in MB */
  int nbytes = size * 1048576;
  /* work out number of ints that fit into nbytes */
  int nelements = nbytes / sizeof(int);
  VMEM *vmp;

  double oh, rt = 0.0;

  struct timespec start, end;
  
  t = (int)time(NULL);

  /* create pmem pool of memory */
  if ((vmp = vmem_create(nvmelocation,
                         nbytes*2)) == NULL) {
    perror("vmem_create");
    exit(1);
  }

  int *array;
  /* allocate 'size' MB of memory */
  if((array = vmem_malloc(vmp, nbytes)) == NULL){
                perror("vmem_malloc");
                exit(1);
  }
  
  if(array == NULL){
    printf("Out Of Memory: could not allocate space for the array.\n");
    return 0;
  }

  srand(t);

  clock_gettime(CLOCK, &start);


  /* measure overheads */
  for(i=0; i<nelements; i++){
    n = rand() % (nelements);
  }

  clock_gettime(CLOCK, &end);
  oh = elapsed_time_hr(start, end, "Overhead for random write");

  printf("Keep result: n = %d\n", n);  
  
  srand(t);
  int data = rand();

  clock_gettime(CLOCK, &start);
  
  
  /* write data to array in random manner */
  for(i = 0; i < nelements; i++){
    n = rand() % (nelements);
    array[n] = data;
  }
    
  clock_gettime(CLOCK, &end);
  rt = elapsed_time_hr(start, end, "Write random");

  printf("Runtime: %f\n", (rt-oh));
  
  printf("Finished allocating %d MB memory (%d ints) and "
	 "filling the array in a random manner.\n\n", size, nelements);
  
  vmem_free(vmp, array); // Remember to free !

  vmem_delete(vmp);

  return 0;
  
}
#endif


/* allocate 'size' amount of memory (in MB)        */
/* write a random value to all the array elements  */
/* in a contiguous manner, read back from memory   */
/* and, finally, free the memory.                  */
int mem_read_contig(unsigned int size){
  
  int i, n;
  /* work out number of bytes from size in MB */
  int nbytes = size * 1048576; 
  /* work out number of ints that fit into nbytes */
  int nelements = nbytes / sizeof(int); 
  
  double oh, rt;

  struct timespec start, end;
  
  srand((int)time(NULL));
  
  int data = rand();
  int receive = 0;

  /* allocate array of 'size' ints and fill it in contiguous manner */
  int *array = (int *)malloc(nbytes);
  
  if(array == NULL){
    printf("Out Of Memory: could not allocate space for the array.\n");
    return 0;
  }

  /* populate array with random values */
  for(i = 0; i < nelements; i++){
    array[i] = data;
  }

  clock_gettime(CLOCK, &start);


  /* loop overhead */
  for(i=0; i<nelements; i++){
    __asm__ ("nop");
  }

  clock_gettime(CLOCK, &end);
  oh = elapsed_time_hr(start, end, "Loop overhead for contiguous read");
  
  clock_gettime(CLOCK, &start);

    
  /* read data back from array */
  for(i = 0; i < nelements; i++){
    receive = array[i];
  }
  
  clock_gettime(CLOCK, &end);
  rt = elapsed_time_hr(start, end, "Read contiguously");
  
  printf("Keep result: receive = %d\n", receive);

  printf("Runtime: %f\n", (rt-oh));  

  printf("Finished reading %d MB memory (%d ints) in a contiguous manner.\n\n", size, nelements);
  
  free(array); // Remember to free !
  
  return 0;
  
}

#ifdef PMEM
/* allocate 'size' amount of memory (in MB) from pmem.io vmem        */
/* write a random value to all the array elements  */
/* in a contiguous manner, read back from memory   */
/* and, finally, free the memory.                  */
int vmem_read_contig(unsigned int size, char *nvmelocation){
  
  int i, n;
  /* work out number of bytes from size in MB */
  int nbytes = size * 1048576; 
  /* work out number of ints that fit into nbytes */
  int nelements = nbytes / sizeof(int); 
  
  double oh, rt;

  struct timespec start, end;
  
  srand((int)time(NULL));
  
  int data = rand();
  int receive = 0;

  /* allocate array of 'size' ints and fill it in contiguous manner */
  int *array = (int *)malloc(nbytes);
  
  if(array == NULL){
    printf("Out Of Memory: could not allocate space for the array.\n");
    return 0;
  }

  /* populate array with random values */
  for(i = 0; i < nelements; i++){
    array[i] = data;
  }

  clock_gettime(CLOCK, &start);


  /* loop overhead */
  for(i=0; i<nelements; i++){
    __asm__ ("nop");
  }

  clock_gettime(CLOCK, &end);
  oh = elapsed_time_hr(start, end, "Loop overhead for contiguous read");
  
  clock_gettime(CLOCK, &start);

    
  /* read data back from array */
  for(i = 0; i < nelements; i++){
    receive = array[i];
  }
  
  clock_gettime(CLOCK, &end);
  rt = elapsed_time_hr(start, end, "Read contiguously");
  
  printf("Keep result: receive = %d\n", receive);

  printf("Runtime: %f\n", (rt-oh));  

  printf("Finished reading %d MB memory (%d ints) in a contiguous manner.\n\n", size, nelements);
  
  free(array); // Remember to free !
  
  return 0;
  
}
#endif

/* allocate memory for 'size' number of elements   */
/* of type 'dt', write a random value to all the   */
/* array elements, read those values back in a     */
/* strided manner and then free the memory.        */
int mem_read_strided(unsigned int size, unsigned int stride){
  
  int i, j, s, n = 0;
  /* work out number of bytes from size in MB */
  unsigned int nbytes = size * 1048576;
  /* work out number of bytes from stride in KB */
  unsigned int strbytes = stride * 1024;

  /* calculate total number of elements to be allocated/read */
  unsigned int nelements = nbytes / sizeof(int);
  /* convert stride size into ints */
  unsigned int str = strbytes / sizeof(int);

  double oh, rt = 0.0;

  struct timespec start, end;

  srand((int)time(NULL));
  
  int data = rand();
  int receive = 0;

  /* perform the strided write for all stride values */
  /* from 2 up to str                                */
  for(s=2; s<=str; s=s*2){

    printf("Memory size in ints: %d, stride in ints: %d\n", nelements, s);
    
    /* allocate array of 'size' ints and write data in contiguous manner */
    int *array = (int *)malloc(nbytes);
    
    if(array == NULL){
      printf("Out Of Memory: could not allocate space for the array.\n");
      return 0;
    }
    
    /* fill array with data - no need for striding here */  
    for(i = 0; i < nelements; i++){
      array[i] = data;
    }

    clock_gettime(CLOCK, &start);

    
    /* measure the overhead */
    for(i = 0; i < s; i++){
      for(j = 0; j < nelements; j=j+s){
	if(i+j < nelements){
	  n = i+j;
	}
	if(n == nelements-1) break;
	n++;
      }
      if(n == nelements-1) break;
    }
    
    clock_gettime(CLOCK, &end);
    oh = elapsed_time_hr(start, end, "Overhead for strided read");
    
    /* make sure the compiler executes the loop that calculates n */
    printf("n = %d\n", n);
    
    clock_gettime(CLOCK, &start);

    
    /* read data from array following strided pattern */  
    for(i = 0; i < s; i++){
      for(j = 0; j < nelements; j=j+s){
	if(i+j < nelements){
	  n = i+j;
	  receive = array[n];
	}
	if(n == nelements-1) break;
	n++;
      }
      if(n == nelements-1) break;
    }
    
    clock_gettime(CLOCK, &end);
    rt = elapsed_time_hr(start, end, "Read strided");
    
    /* make sure the compiler executes the loop that calculates n */
    printf("n = %d, receive = %d\n", n, receive);
    
    printf("Runtime: %f\n", (rt-oh));
    
    free(array); // Remember to free !
  }    
  
  printf("Finished allocating %d MB memory and "
	 "reading the array in a strided manner "
	 "with a stride size of %d KB.\n\n", size, stride);

  return 0;
  
}

#ifdef PMEM
/* allocate memory for 'size' number of elements   */
/* of type 'dt', write a random value to all the   */
/* array elements, read those values back in a     */
/* strided manner and then free the memory.        */
int vmem_read_strided(unsigned int size, unsigned int stride, char *nvmelocation){
  
  int i, j, s, n = 0;
  /* work out number of bytes from size in MB */
  unsigned int nbytes = size * 1048576;
  /* work out number of bytes from stride in KB */
  unsigned int strbytes = stride * 1024;

  /* calculate total number of elements to be allocated/read */
  unsigned int nelements = nbytes / sizeof(int);
  /* convert stride size into ints */
  unsigned int str = strbytes / sizeof(int);

  double oh, rt = 0.0;

  struct timespec start, end;

  srand((int)time(NULL));
  
  int data = rand();
  int receive = 0;

  /* perform the strided write for all stride values */
  /* from 2 up to str                                */
  for(s=2; s<=str; s=s*2){

    printf("Memory size in ints: %d, stride in ints: %d\n", nelements, s);
    
    /* allocate array of 'size' ints and write data in contiguous manner */
    int *array = (int *)malloc(nbytes);
    
    if(array == NULL){
      printf("Out Of Memory: could not allocate space for the array.\n");
      return 0;
    }
    
    /* fill array with data - no need for striding here */  
    for(i = 0; i < nelements; i++){
      array[i] = data;
    }

    clock_gettime(CLOCK, &start);

    
    /* measure the overhead */
    for(i = 0; i < s; i++){
      for(j = 0; j < nelements; j=j+s){
	if(i+j < nelements){
	  n = i+j;
	}
	if(n == nelements-1) break;
	n++;
      }
      if(n == nelements-1) break;
    }
    
    clock_gettime(CLOCK, &end);
    oh = elapsed_time_hr(start, end, "Overhead for strided read");
    
    /* make sure the compiler executes the loop that calculates n */
    printf("n = %d\n", n);
    
    clock_gettime(CLOCK, &start);

    
    /* read data from array following strided pattern */  
    for(i = 0; i < s; i++){
      for(j = 0; j < nelements; j=j+s){
	if(i+j < nelements){
	  n = i+j;
	  receive = array[n];
	}
	if(n == nelements-1) break;
	n++;
      }
      if(n == nelements-1) break;
    }
    
    clock_gettime(CLOCK, &end);
    rt = elapsed_time_hr(start, end, "Read strided");
    
    /* make sure the compiler executes the loop that calculates n */
    printf("n = %d, receive = %d\n", n, receive);
    
    printf("Runtime: %f\n", (rt-oh));
    
    free(array); // Remember to free !
  }    
  
  printf("Finished allocating %d MB memory and "
	 "reading the array in a strided manner "
	 "with a stride size of %d KB.\n\n", size, stride);

  return 0;
  
}
#endif

/* allocate memory for 'size' number of elements   */
/* of type 'dt', write a random value to all the   */
/* array elements, read those values back in a     */
/* random manner and then free the memory.        */
int mem_read_random(unsigned int size){
  
  int i, n;
  unsigned int t;

  double oh, rt;

  /* work out number of bytes from size in MB */
  unsigned int nbytes = size * 1048576;
  /* calculate total number of elements to be allocated/read */
  unsigned int nelements = nbytes / sizeof(int);
  
  struct timespec start, end;
  
  t =(int)time(NULL);

  int data = rand();
  int receive = 0;
  
  /* allocate array of 'size' ints and write data in contiguous manner */
  int *array = (int *)malloc(nbytes);
  
  if(array == NULL){
    printf("Out Of Memory: could not allocate space for the array.\n");
    return 0;
  }
  
  /* fill the array with random values */
  for(i = 0; i < nelements; i++){
    array[i] = data;
  }

  srand(t);

  /* measure the overhead */
  clock_gettime(CLOCK, &start);


  for(i=0; i<nelements; i++){
    n = rand() % (nelements);
  }

  clock_gettime(CLOCK, &end);
  oh = elapsed_time_hr(start, end, "Read random");
  
  printf("Make sure the result is kept: %d\n", n);

  srand(t);

  /* measure the random reads */
  clock_gettime(CLOCK, &start);

  
  for(i = 0; i < nelements; i++){
    n = rand() % (nelements);
    receive = array[n];
  }
  
  clock_gettime(CLOCK, &end);
  rt = elapsed_time_hr(start, end, "Read random");
  
  printf("Make sure the result is kept: %d\n", receive);

  printf("Runtime: %f\n", (rt-oh));

  printf("Finished reading %d MB of memory (%d ints) in a random manner.\n\n", size, nelements);
  
  free(array); // Remember to free !

  return 0;
  
}

#ifdef PMEM
/* allocate memory for 'size' number of elements   */
/* of type 'dt', write a random value to all the   */
/* array elements, read those values back in a     */
/* random manner and then free the memory.        */
int vmem_read_random(unsigned int size, char *nvmelocation){
  
  int i, n;
  unsigned int t;

  double oh, rt;

  /* work out number of bytes from size in MB */
  unsigned int nbytes = size * 1048576;
  /* calculate total number of elements to be allocated/read */
  unsigned int nelements = nbytes / sizeof(int);
  
  struct timespec start, end;
  
  t =(int)time(NULL);

  int data = rand();
  int receive = 0;
  
  /* allocate array of 'size' ints and write data in contiguous manner */
  int *array = (int *)malloc(nbytes);
  
  if(array == NULL){
    printf("Out Of Memory: could not allocate space for the array.\n");
    return 0;
  }
  
  /* fill the array with random values */
  for(i = 0; i < nelements; i++){
    array[i] = data;
  }

  srand(t);

  /* measure the overhead */
  clock_gettime(CLOCK, &start);


  for(i=0; i<nelements; i++){
    n = rand() % (nelements);
  }

  clock_gettime(CLOCK, &end);
  oh = elapsed_time_hr(start, end, "Read random");
  
  printf("Make sure the result is kept: %d\n", n);

  srand(t);

  /* measure the random reads */
  clock_gettime(CLOCK, &start);

  
  for(i = 0; i < nelements; i++){
    n = rand() % (nelements);
    receive = array[n];
  }
  
  clock_gettime(CLOCK, &end);
  rt = elapsed_time_hr(start, end, "Read random");
  
  printf("Make sure the result is kept: %d\n", receive);

  printf("Runtime: %f\n", (rt-oh));

  printf("Finished reading %d MB of memory (%d ints) in a random manner.\n\n", size, nelements);
  
  free(array); // Remember to free !

  return 0;
  
}
#endif

void swap (int *a, int x, int y)
{
  int t;
  t = a[x];
  a[x] = a[y];
  a[y] = t;
}

/* Read latency */ 
int read_lat(unsigned int size){
  
  struct timespec start, end;

  int delta = 400;
  // convert from MB to B and calculate number of ints
  int L = size*1024*1024/sizeof(int); 
  int i = 0, ii = 0, x = 0, y = 0, c = 0;
  int pid = getpid();
  double tempTime = 0.0;
  int *array;
  int intsPerCacheLine, cacheLineSize;

  cacheLineSize=(int)sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
  srand48 ((long int) pid);
  
  intsPerCacheLine = cacheLineSize / sizeof(int);
#ifdef DEBUG
  printf("cache line size = %d\n", cacheLineSize);
  printf("perCacheLine: %iBytes, sizeofInt: %iBytes, arraySize: %iBytes\n", intsPerCacheLine, (int) sizeof(int), (int) L*sizeof(int));
#endif
  
  /* allocate & zero */
  array = (int *) calloc (L, sizeof (int));
  
  for (i = 0; i < L; i = i + intsPerCacheLine){
    /* assign each element the index value of the next element */
    array[i] = i + intsPerCacheLine;   
  }
  array[L - intsPerCacheLine] = 0; // circular buffer
  
  for (i = 0; i <= (L - intsPerCacheLine); i = i + intsPerCacheLine){
    int range = L - intsPerCacheLine - i;
    int smallr = range / intsPerCacheLine;
    
    c = (i + (int) (drand48() * range / intsPerCacheLine) * intsPerCacheLine);
    x = array[i];
    y = array[c];
    swap (array, i, c);
    swap (array, x, y);
  }
  
  // Warm-up caches / First Time Effect
  int p = array[0];
  while (p != 0){
    p = array[p];
  }
  
  clock_gettime(CLOCK, &start);
  p = array[0];
  while (p != 0){
    p = array[p];
  }
  clock_gettime(CLOCK, &end);
  
  /* hops = number of array elements read */
  int hops = ( L / intsPerCacheLine ) - 1;
  /* latency = time / hops */
  tempTime = elapsed_time_hr(start, end, "Read latency");
  
  printf("Number of hops: %d\n", hops);
  printf("Latency: %f us\n", 1.0e+9 * tempTime/(double)hops);
  
  free(array);

  return 0;
}

#ifdef PMEM
/* Read latency for pmem.io vmem*/
int vmem_read_lat(unsigned int size, char *nvmelocation){

  struct timespec start, end;

  int delta = 400;
  // convert from MB to B and calculate number of ints
  int L = size*1024*1024/sizeof(int);
  int i = 0, ii = 0, x = 0, y = 0, c = 0;
  int pid = getpid();
  double tempTime = 0.0;
  int *array;
  int intsPerCacheLine, cacheLineSize;
  VMEM *vmp;

  cacheLineSize=(int)sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
  srand48 ((long int) pid);

  intsPerCacheLine = cacheLineSize / sizeof(int);
#ifdef DEBUG
  printf("cache line size = %d\n", cacheLineSize);
  printf("perCacheLine: %iBytes, sizeofInt: %iBytes, arraySize: %iBytes\n", intsPerCacheLine, (int) sizeof(int), (int) L*sizeof(int));
#endif

  /* create pmem pool of memory */
  if ((vmp = vmem_create(nvmelocation,
                         L*sizeof(int)*2)) == NULL) {
    perror("vmem_create");
    exit(1);
  }

  /* allocate array */
  if((array = (int *)vmem_calloc(vmp, L, sizeof(int))) == NULL){
                perror("vmem_calloc");
                exit(1);
  }


  for (i = 0; i < L; i = i + intsPerCacheLine){
    /* assign each element the index value of the next element */
    array[i] = i + intsPerCacheLine;
  }
  array[L - intsPerCacheLine] = 0; // circular buffer

  for (i = 0; i <= (L - intsPerCacheLine); i = i + intsPerCacheLine){
    int range = L - intsPerCacheLine - i;
    int smallr = range / intsPerCacheLine;

    c = (i + (int) (drand48() * range / intsPerCacheLine) * intsPerCacheLine);
    x = array[i];
    y = array[c];
    swap (array, i, c);
    swap (array, x, y);
  }

  // Warm-up caches / First Time Effect
  int p = array[0];
  while (p != 0){
    p = array[p];
  }

  clock_gettime(CLOCK, &start);
  p = array[0];
  while (p != 0){
    p = array[p];
  }

  clock_gettime(CLOCK, &start);
  p = array[0];
  while (p != 0){
    p = array[p];
  }
  clock_gettime(CLOCK, &end);

  /* hops = number of array elements read */
  int hops = ( L / intsPerCacheLine ) - 1;
  /* latency = time / hops */
  tempTime = elapsed_time_hr(start, end, "Read latency");

  printf("Number of hops: %d\n", hops);
  printf("Latency: %f us\n", 1.0e+9 * tempTime/(double)hops);

  vmem_free(vmp, array);

  vmem_delete(vmp);

  return 0;
}
#endif
