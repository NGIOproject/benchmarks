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

/* need this to get O_DIRECT definition */
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

#include "utils.h"

#ifdef PMEM
#include<libpmem.h>
#endif


int mk_rm_dir(unsigned int N){

  char d[32]; 
  int i = 0, nanosleepInterval = 0; 

  struct timespec start, end;
  struct timespec timeToSleep, timeRemaining;
  timeToSleep.tv_sec  = 0;
  timeToSleep.tv_nsec = 250000000L;    // 0.25 seconds

  /* if N has been left at 2 million by mistake, reduce */ 
  /* to 100k to avoid creating too many directories.    */
  if(N == 2000000) N = 100000;

  /* warm-up */
  for(i=0; i<100;i++){
    sprintf(d, "warmupdir_%d", i);
    mkdir(d,777);
  }

  clock_gettime(CLOCK, &start);

  /* create directories */
  for(i=0; i<N;i++){
    sprintf(d, "testdir_%d", i);
    mkdir(d,777);
    if ( i % 10000 == 0 )
    {
        nanosleep(&timeToSleep, &timeRemaining);
        nanosleepInterval++;
    }
  }

  clock_gettime(CLOCK, &end);
  elapsed_time_hr(start, end, "mkdir");
  printf("Slept for :%6.3f, ms.\n", 1000*nanosleepInterval*0.25);
  nanosleepInterval = 0;

  sleep(5);

  /* warm-up */
  for(i=0; i<100;i++){
    sprintf(d, "warmupdir_%d", i);
    rmdir(d);
  }
  clock_gettime(CLOCK, &start);

  /* remove previously created directories */
  for(i=0; i<N;i++){
    sprintf(d, "testdir_%d", i);
    rmdir(d);
    if ( i % 10000 == 0 )
    {
        nanosleep(&timeToSleep, &timeRemaining);
        nanosleepInterval++;
    }
  }

  clock_gettime(CLOCK, &end);
  elapsed_time_hr(start, end, "rmdir");
  printf("Slept for :%6.3f, ms.\n", 1000*nanosleepInterval*0.25);

  fflush(stdout);
  return 0;
}




int file_write(unsigned int N)
{
    struct timespec start, end;
    char name[100];
    int size = 1;
    int i, j;
    unsigned char *data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int reps;

    /* allocate and initialise data */
    data = malloc(N);
    if (!data) {
	fprintf(stderr, "ERROR: out of memory in file_write\n");
	return 1;
    }
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_write\n");
	return 1;
    }
    read(fd, data, N);
    close(fd);

    warmupsize = 10000;
    if (warmupsize > N) warmupsize = N;

    /* do warm-up */
    for (i = 0; i < 100; i++) {
	sprintf(name, "warmup_%d", i);
	fd = open(name, O_CREAT|O_WRONLY|O_TRUNC, 0644);
	if (fd < 0) {
	    fprintf(stderr, "ERROR: unable to open warmup file for writing in file_write\n");
	    return 1;
	}
	write(fd, data, warmupsize);
	close(fd);
    }

    /* don't overload the system by doing too many tiny files */
    while (N >= 10000) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {
	sprintf(titlebuffer, "file_write: %d files of %d bytes", N, size);

	/* do actual write test */
	clock_gettime(CLOCK, &start);

	reps = 128 / N;
	if (reps == 0) reps = 1;

	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
		sprintf(name, "testfile_%d", i);
		
		fd = open(name, O_CREAT|O_WRONLY|O_TRUNC, 0644);
		if (fd < 0) {
		    fprintf(stderr, "ERROR: unable to open test file for writing in file_write\n");
		    return 1;
		}
		write(fd, data, size);
		fsync(fd);
		close(fd);
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* remove files just created */
	for (i = 0; i < N; i++) {
	    sprintf(name, "testfile_%d", i);
	    unlink(name);
	}

	/* halve number of files but double their size */
	N = N / 2;
	size = size * 2;
    }

    /* remove warm-up files */
    for (i = 0; i < 100; i++) {
	sprintf(name, "warmup_%d", i);
	unlink(name);
    }

    free(data);
    fflush(stdout);
    return 0;
}


#ifdef PMEM
int pmem_file_write(unsigned int N, char *nvmelocation){

    struct timespec start, end;
    char name[100]="";
    char origname[100]="";
    char number[5];
    int size = 1;
    int i, j;
    unsigned char *data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int reps;

    char *pmemaddr, *origpmemaddr;
    size_t mapped_len;
    int is_pmem;

    strcpy(name, nvmelocation);
    strcat(name, "/");
    strcpy(origname,name);

    /* allocate and initialise data */
    data = malloc(N);
    if (!data) {
	fprintf(stderr, "ERROR: out of memory in file_write\n");
	return 1;
    }
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_write\n");
	return 1;
    }
    read(fd, data, N);
    close(fd);

    warmupsize = 10000;
    if (warmupsize > N) warmupsize = N;

    /* do warm-up */
    for (i = 0; i < 100; i++) {
	sprintf(number, "%d", i);
	strcpy(name,origname);
	strcat(name,"warmup_");
	strcat(name,number);
        if ((pmemaddr = pmem_map_file(name, warmupsize, PMEM_FILE_CREATE,
                                0644, &mapped_len, &is_pmem)) == NULL) {
                perror("ERROR: unable to open warmup file for writing in pmem_file_write\n");
                exit(1);
        }
	origpmemaddr = pmemaddr;
	pmem_memcpy_nodrain(pmemaddr, data, warmupsize);
	pmem_drain();
	pmem_unmap(origpmemaddr, mapped_len);
    }

    /* don't overload the system by doing too many tiny files */
    while (N >= 10000) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {
	sprintf(titlebuffer, "file_write: %d files of %d bytes", N, size);

	/* do actual write test */
	clock_gettime(CLOCK, &start);

	reps = 128 / N;
	if (reps == 0) reps = 1;

	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
	      sprintf(number, "%d", i);
	      strcpy(name,origname);
	      strcat(name,"testfile_");
	      strcat(name,number);
	      if ((pmemaddr = pmem_map_file(name, size, PMEM_FILE_CREATE,
					    0644, &mapped_len, &is_pmem)) == NULL) {
                perror("ERROR: unable to open test file for writing in pmem_file_write\n");
                exit(1);
	      }
	      origpmemaddr = pmemaddr;
	      pmem_memcpy_nodrain(pmemaddr, data, size);
	      pmem_drain();
	      pmem_unmap(origpmemaddr, mapped_len);
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* remove files just created */
	for (i = 0; i < N; i++) {
	  sprintf(number, "%d", i);
	  strcpy(name,origname);
	  strcat(name,"testfile_");
	  strcat(name,number);	  
	  unlink(name);
	}

	/* halve number of files but double their size */
	N = N / 2;
	size = size * 2;
    }

    /* remove warm-up files */
    for (i = 0; i < 100; i++) {
      sprintf(number, "%d", i);
      strcpy(name,origname);
      strcat(name,"warmup_");
      strcat(name,number);	  
      unlink(name);
    }

    free(data);
    fflush(stdout);
    return 0;

}
#endif


int file_write_random(unsigned int N)
{
    struct timespec start, end;
    char name[100];
    int size = 1;
    int i, j;
    unsigned char *data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int block;
    int reps;

    /* allocate and initialise data */
    data = malloc(N);
    if (!data) {
	fprintf(stderr, "ERROR: out of memory in file_write_random\n");
	return 1;
    }
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_write_random\n");
	return 1;
    }
    read(fd, data, N);
    close(fd);

    /* now create the test file */
    fd = open("testfile_1", O_CREAT|O_WRONLY|O_TRUNC, 0644);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open test file for writing in file_write_random\n");
	return 1;
    }
    write(fd, data, N);
    fsync(fd);
    close(fd);

    /* don't overload the system by doing too many tiny blocks */
    while (N >= 10000) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {

	sprintf(titlebuffer, "file_write_random: %d blocks of %d bytes", N, size);

	clock_gettime(CLOCK, &start);

	reps = 128 / N;
	if (reps == 0) reps = 1;

	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
		/* open the big test file for writing */
		fd = open("testfile_1", O_WRONLY);
		if (fd < 0) {
		    fprintf(stderr, "ERROR: unable to open test file for writing in file_write_random\n");
		    return 1;
		}
		
		/* choose a random block within the file */
		block = rand() % N;
		
		/* write to that block */
		lseek(fd, block * size, SEEK_SET);
		write(fd, data, size);
		fsync(fd);
		close(fd);
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* halve number of blocks but double their size */
	N = N / 2;
	size = size * 2;
    }

    /* clean up the test file */
    unlink("testfile_1");
    free(data);
    fflush(stdout);

    return 0;
}

#ifdef PMEM
int pmem_file_write_random(unsigned int N, char *nvmelocation)
{
    struct timespec start, end;
    char name[100]="";
    char origname[100]="";
    int size = 1;
    int i, j;
    unsigned char *data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int block;
    int reps;
    int charsize;

    char *pmemaddr, *origpmemaddr;
    size_t mapped_len;
    int is_pmem;

    charsize = sizeof(char);

    strcpy(name, nvmelocation);
    strcat(name, "/");
    strcpy(origname,name);

    /* allocate and initialise data */
    data = malloc(N);
    if (!data) {
	fprintf(stderr, "ERROR: out of memory in pmem_file_write_random\n");
	return 1;
    }
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in pmem_file_write_random\n");
	return 1;
    }
    read(fd, data, N);
    close(fd);

    strcpy(name,origname);
    strcat(name,"testfile_1");
    if ((pmemaddr = pmem_map_file(name, N, PMEM_FILE_CREATE,
				  0644, &mapped_len, &is_pmem)) == NULL) {
      perror("ERROR: unable to open warmup file for writing in pmem_file_write_random\n");
      exit(1);
    }
    origpmemaddr = pmemaddr;
    pmem_memcpy_nodrain(pmemaddr, data, N);
    pmem_drain();
    pmem_unmap(origpmemaddr, mapped_len);


    /* don't overload the system by doing too many tiny blocks */
    while (N >= 10000) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {

	sprintf(titlebuffer, "pmem_file_write_random: %d blocks of %d bytes", N, size);

	clock_gettime(CLOCK, &start);

	reps = 128 / N;
	if (reps == 0) reps = 1;

	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {

	      /* choose a random block within the file */
	      block = rand() % N;
	      
	      /* open the big test file for writing */
	      strcpy(name,origname);
	      strcat(name,"testfile_1");
	      if ((pmemaddr = pmem_map_file(name, 0, 0,
					    0644, &mapped_len, &is_pmem)) == NULL) {
		perror("ERROR: unable to open test file for writing in pmem_file_write_random\n");
		exit(1);
	      }
	      origpmemaddr = pmemaddr;
	      pmemaddr += block * size * charsize;
	      pmem_memcpy_nodrain(pmemaddr, data, size);
	      pmem_drain();
	      pmem_unmap(origpmemaddr, mapped_len);

	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* halve number of blocks but double their size */
	N = N / 2;
	size = size * 2;
    }

    /* clean up the test file */
    strcpy(name,origname);
    strcat(name,"testfile_1");
    unlink(name);
    free(data);
    fflush(stdout);

    return 0;
}
#endif

int file_read(unsigned int N)
{
    struct timespec start, end;
    char name[100];
    int size = 1;
    int i, j;
    unsigned char *data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int reps;

    /* allocate and initialise test data */
    data = malloc(N);
    if (!data) {
	fprintf(stderr, "ERROR: out of memory in file_read\n");
	return 1;
    }
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_read\n");
	return 1;
    }
    read(fd, data, N);
    close(fd);

    /* don't overload the system by doing too many tiny files */
    while (N >= 10000) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {
	sprintf(titlebuffer, "file_read: %d files of %d bytes", N, size);

	/* first create files to read later */
	for (i = 0; i < N; i++) {
	    sprintf(name, "testfile_%d", i);

	    fd = open(name, O_CREAT|O_WRONLY|O_TRUNC, 0644);
	    if (fd < 0) {
		fprintf(stderr, "ERROR: unable to open test file for writing in file_read\n");
		return 1;
	    }
	    write(fd, data, size);
	    fsync(fd);
	    close(fd);
	}

	reps = 32768 / N;
	if (reps == 0) reps = 1;

	/* now do read test */
	clock_gettime(CLOCK, &start);
	
	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
		sprintf(name, "testfile_%d", i);
		
		fd = open(name, O_RDONLY);
		if (fd < 0) {
		    fprintf(stderr, "ERROR: unable to open test file for reading in file_read\n");
		    return 1;
		}
		read(fd, data, size);
		close(fd);
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* remove files just created */
	for (i = 0; i < N; i++) {
	    sprintf(name, "testfile_%d", i);
	    unlink(name);
	}

	/* halve number of files but double their size */
	N = N / 2;
	size = size * 2;
    }
    free(data);
    fflush(stdout);

    return 0;
}

#ifdef PMEM
int pmem_file_read(unsigned int N, char *nvmelocation)
{
    struct timespec start, end;
    char name[100]="";
    char origname[100]="";
    char number[5];
    int size = 1;
    int i, j;
    unsigned char *data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int reps;

    char *pmemaddr, *origpmemaddr;
    size_t mapped_len;
    int is_pmem;

    strcpy(name, nvmelocation);
    strcat(name, "/");
    strcpy(origname,name);

    /* allocate and initialise test data */
    data = malloc(N);
    if (!data) {
	fprintf(stderr, "ERROR: out of memory in pmem_file_read\n");
	return 1;
    }
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in pmem_file_read\n");
	return 1;
    }
    read(fd, data, N);
    close(fd);

    /* don't overload the system by doing too many tiny files */
    while (N >= 10000) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {
	sprintf(titlebuffer, "file_read: %d files of %d bytes", N, size);

	/* first create files to read later */
	for (i = 0; i < N; i++) {
	  sprintf(number, "%d", i);
	  strcpy(name,origname);
	  strcat(name,"testfile_");
	  strcat(name,number);
	  if ((pmemaddr = pmem_map_file(name, size, PMEM_FILE_CREATE,
					0644, &mapped_len, &is_pmem)) == NULL) {
	    perror("ERROR: unable to open test file for writing in pmem_file_read\n");
	    exit(1);
	  }
	  origpmemaddr = pmemaddr;
	  pmem_memcpy_nodrain(pmemaddr, data, warmupsize);
	  pmem_drain();
	  pmem_unmap(origpmemaddr, mapped_len);
	}

	reps = 32768 / N;
	if (reps == 0) reps = 1;

	/* now do read test */
	clock_gettime(CLOCK, &start);
	
	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
	      sprintf(number, "%d", i);
	      strcpy(name,origname);
	      strcat(name,"testfile_");
	      strcat(name,number);
	      if ((pmemaddr = pmem_map_file(name, 0, 0,
					    0644, &mapped_len, &is_pmem)) == NULL) {
		perror("ERROR: unable to open test file for writing in pmem_file_read\n");
		exit(1);
	      }
	      origpmemaddr = pmemaddr;
	      memcpy(data, pmemaddr, size);
	      pmem_unmap(origpmemaddr, mapped_len);
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* remove files just created */
	for (i = 0; i < N; i++) {
	  sprintf(number, "%d", i);
	  strcpy(name,origname);
	  strcat(name,"testfile_");
	  strcat(name,number);	 
	  unlink(name);
	}

	/* halve number of files but double their size */
	N = N / 2;
	size = size * 2;
    }
    free(data);
    fflush(stdout);

    return 0;
}
#endif

#ifndef __MACH__
int file_read_direct(unsigned int N)
{
    struct timespec start, end;
    char name[100];
    int size = 1;
    int i, j;
    unsigned char *data_orig, *data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int reps;

    /* allocate buffer and align to 512 byte boundary */
    data_orig = malloc(N + 512);
    if (!data_orig) {
	fprintf(stderr, "ERROR: out of memory in file_read_direct\n");
	return 1;
    }
    data = (unsigned char *)(((long)data_orig + 511) & (~511));

    /* initialise test data */
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_read_direct\n");
	return 1;
    }
    read(fd, data, N);
    close(fd);

    /* N needs to be a multiple of 512 */
    while (size < 512) {
	N = N / 2;
	size = size * 2;
    }


    while (N >= 1) {
	sprintf(titlebuffer, "file_read_direct: %d files of %d bytes", N, size);

	/* first create files to read later */
	for (i = 0; i < N; i++) {
	    sprintf(name, "testfile_%d", i);

	    fd = open(name, O_CREAT|O_WRONLY|O_TRUNC, 0644);
	    if (fd < 0) {
		fprintf(stderr, "ERROR: unable to open test file for writing in file_read_direct\n");
		return 1;
	    }
	    write(fd, data, size);
	    fsync(fd);
	    close(fd);
	}

	reps = 2048 / N;
	if (reps == 0) reps = 1;

	/* now do read test */
	clock_gettime(CLOCK, &start);

	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
		sprintf(name, "testfile_%d", i);
		
		fd = open(name, O_RDONLY|O_DIRECT);
		if (fd < 0) {
		    fprintf(stderr, "ERROR: unable to open test file for reading in file_read_direct\n");
		    return 1;
		}
		read(fd, data, size);
		close(fd);
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* remove files just created */
	for (i = 0; i < N; i++) {
	    sprintf(name, "testfile_%d", i);
	    unlink(name);
	}

	/* halve number of files but double their size */
	N = N / 2;
	size = size * 2;
    }
    free(data_orig);
    fflush(stdout);

    return 0;
}
#endif

int file_read_random(unsigned int N)
{
    struct timespec start, end;
    char name[100];
    int size = 1;
    int i, j;
    unsigned char *data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int block;
    int reps;

    /* allocate and initialise data */
    data = malloc(N);
    if (!data) {
	fprintf(stderr, "ERROR: out of memory in file_read_random\n");
	return 1;
    }
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_read_random\n");
	return 1;
    }
    read(fd, data, N);
    close(fd);

    /* now create the test file */
    fd = open("testfile_1", O_CREAT|O_WRONLY|O_TRUNC, 0644);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open test file for writing in file_read_random\n");
	return 1;
    }
    write(fd, data, N);
    fsync(fd);
    close(fd);

    /* don't overload the system by doing too many tiny blocks */
    while (N >= 10000) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {

	sprintf(titlebuffer, "file_read_random: %d blocks of %d bytes", N, size);

	reps = 65536 / N;
	if (reps == 0) reps = 1;

	clock_gettime(CLOCK, &start);

	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
		/* open the big test file for reading */
		fd = open("testfile_1", O_RDONLY);
		if (fd < 0) {
		    fprintf(stderr, "ERROR: unable to open test file for reading in file_read_random\n");
		    return 1;
		}
		
		/* choose a random block within the file */
		block = rand() % N;
		
		/* write to that block */
		lseek(fd, block * size, SEEK_SET);
		read(fd, data, size);
		close(fd);
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* halve number of blocks but double their size */
	N = N / 2;
	size = size * 2;
    }

    /* clean up the test file */
    unlink("testfile_1");
    free(data);
    fflush(stdout);

    return 0;
}

#ifdef PMEM
int pmem_file_read_random(unsigned int N, char *nvmelocation)
{
    struct timespec start, end;
    char name[100]="";
    char origname[100]="";
    int size = 1;
    int i, j;
    unsigned char *data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int block;
    int reps;
    int charsize;

    char *pmemaddr, *origpmemaddr;
    size_t mapped_len;
    int is_pmem;

    charsize = sizeof(char);

    strcpy(name, nvmelocation);
    strcat(name, "/");
    strcpy(origname,name);

    /* allocate and initialise data */
    data = malloc(N);
    if (!data) {
	fprintf(stderr, "ERROR: out of memory in file_read_random\n");
	return 1;
    }
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_read_random\n");
	return 1;
    }
    read(fd, data, N);
    close(fd);

    /* now create the test file */
    strcpy(name,origname);
    strcat(name,"testfile_1");
    if ((pmemaddr = pmem_map_file(name, N, PMEM_FILE_CREATE,
				  0644, &mapped_len, &is_pmem)) == NULL) {
      perror("ERROR: unable to open warmup file for writing in pmem_file_read_random\n");
      exit(1);
    }
    origpmemaddr = pmemaddr;
    pmem_memcpy_nodrain(pmemaddr, data, N);
    pmem_drain();
    pmem_unmap(origpmemaddr, mapped_len);

    /* don't overload the system by doing too many tiny blocks */
    while (N >= 10000) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {

	sprintf(titlebuffer, "pmem_file_read_random: %d blocks of %d bytes", N, size);

	reps = 65536 / N;
	if (reps == 0) reps = 1;

	clock_gettime(CLOCK, &start);

	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {

	      /* choose a random block within the file */
	      block = rand() % N;
	      
	      /* open the big test file for writing */
	      strcpy(name,origname);
	      strcat(name,"testfile_1");
	      if ((pmemaddr = pmem_map_file(name, 0, 0,
					    0644, &mapped_len, &is_pmem)) == NULL) {
		perror("ERROR: unable to open test file for writing in pmem_file_read_random\n");
		exit(1);
	      }
	      origpmemaddr = pmemaddr;
	      pmemaddr += block * size * charsize;
	      memcpy(data, pmemaddr, size);
	      pmem_unmap(origpmemaddr, mapped_len);
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* halve number of blocks but double their size */
	N = N / 2;
	size = size * 2;
    }

    /* clean up the test file */
    strcpy(name,origname);
    strcat(name,"testfile_1");
    unlink(name);
    free(data);
    fflush(stdout);

    return 0;
}
#endif

#ifndef __MACH__
int file_read_random_direct(unsigned int N)
{
    struct timespec start, end;
    char name[100];
    int size = 1;
    int i, j;
    unsigned char *data, *data_orig;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int block;
    int reps;

    /* allocate and align buffer */
    data_orig = malloc(N + 512);
    if (!data_orig) {
	fprintf(stderr, "ERROR: out of memory in file_read_random_direct\n");
	return 1;
    }
    data = (unsigned char *)(((long)data_orig + 511) & (~511));

    /* initialise data */
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_read_random_direct\n");
	return 1;
    }
    read(fd, data, N);
    close(fd);

    /* now create the test file */
    fd = open("testfile_1", O_CREAT|O_WRONLY|O_TRUNC, 0644);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open test file for writing in file_read_random_direct\n");
	return 1;
    }
    write(fd, data, N);
    fsync(fd);
    close(fd);

    /* size needs to be 512 byte aligned */
    while (size < 512) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {

	sprintf(titlebuffer, "file_read_random_direct: %d blocks of %d bytes", N, size);

	reps = 512 / N;
	if (reps == 0) reps = 1;

	clock_gettime(CLOCK, &start);

	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
		/* open the big test file for reading */
		fd = open("testfile_1", O_RDONLY|O_DIRECT);
		if (fd < 0) {
		    fprintf(stderr, "ERROR: unable to open test file for reading in file_read_random_direct\n");
		    return 1;
		}
		
		/* choose a random block within the file */
		block = rand() % N;
		
		/* write to that block */
		lseek(fd, block * size, SEEK_SET);
		read(fd, data, size);
		close(fd);
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* halve number of blocks but double their size */
	N = N / 2;
	size = size * 2;
    }

    /* clean up the test file */
    unlink("testfile_1");
    free(data_orig);
    fflush(stdout);

    return 0;
}
#endif
