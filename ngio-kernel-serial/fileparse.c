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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#include "utils.h"
#include "level1.h"

#ifdef PMEM
#include <libpmem.h>
#endif

int create_line(char*, size_t, char*, unsigned int);
int seek_match(char*, size_t, char*, unsigned int);


void fileparse(unsigned int num_rows){

  char search_phrase[] = "AdeptProject";
  size_t sp_len = strlen(search_phrase);

  unsigned int desired_line_len = 81;
  char line[desired_line_len];

  srand(time(NULL)); // Set seed

  int i = 0;
  int r = 0;
  int m = 0;
  int mismatch = 0;
  int r_count = 0;
  int m_count = 0;
  struct timespec start, end;

  FILE* fp;
  fp = fopen("testfile", "w+");

  for (i=0;i<num_rows;i++){
    r = create_line(search_phrase, sp_len, line, desired_line_len);
    m = seek_match(search_phrase, sp_len, line, desired_line_len);
    if (r!=m){
      mismatch++;
    }
    if (r==0){
      r_count++;
    }
    fprintf(fp, "%s\n", line);
  }
  fsync(fileno(fp));
  fclose(fp);

  m=0;

  clock_gettime(CLOCK, &start);
  fp = fopen("testfile", "r");
  while (fscanf(fp, "%s\n", line)!=EOF){
    m = seek_match(search_phrase, sp_len, line, desired_line_len);
    if (m==0){
      m_count++;
    }
  }
  fclose(fp);
  clock_gettime(CLOCK, &end);
  elapsed_time_hr(start, end, "Fileparse");
  if(m_count != r_count || m_count == 0){
    printf("Mismatched counts, test did not pass\n");
  }

  unlink("testfile"); // Use this to ensure the generated file is removed from the system upon finish

}

#ifdef PMEM
void pmem_fileparse(unsigned int num_rows, char *nvmelocation){

  char search_phrase[] = "NEXTGenIOProject";
  size_t sp_len = strlen(search_phrase);

  unsigned int desired_line_len = 81;
  char line[desired_line_len];

  srand(time(NULL)); // Set seed

  int i = 0;
  int r = 0;
  int m = 0;
  int size;
  int mismatch = 0;
  int r_count = 0;
  int m_count = 0;
  int is_pmem = 0;
  struct timespec start, end;
  char  *location;
  char *pmemaddr = NULL;
  char *origpmemaddr;
  size_t mapped_len;
  // Make sure we can calculate very large file sizes
  long long total_size;

  location = malloc(strlen(nvmelocation)+9);
  strcpy(location, nvmelocation);
  strcat(location, "/testfile");

  size = sizeof(char);

  // This is done to enable very large files (larger than 4GB)
  // to be specified (i.e. to stop C defaulting to 32-bit arithmetic on 
  // most machines).
  total_size = desired_line_len;
  total_size *= num_rows;
  total_size *= size;

  if ((pmemaddr = pmem_map_file(location, total_size,
				PMEM_FILE_CREATE|PMEM_FILE_EXCL,
				0666, &mapped_len, &is_pmem)) == NULL) {
    perror("pmem_map_file");
    fprintf(stderr, "Failed to pmem_map_file for filename:%s.\n", location);
    exit(-100);
  }
  origpmemaddr = pmemaddr;

  for (i=0;i<num_rows;i++){
    r = create_line(search_phrase, sp_len, line, desired_line_len);
    m = seek_match(search_phrase, sp_len, line, desired_line_len);
    if (r!=m){
      mismatch++;
    }
    if (r==0){
      r_count++;
    }
    pmem_memcpy_nodrain(pmemaddr, line, desired_line_len * size);
    pmemaddr += desired_line_len * size;
  }
 
  pmem_drain();

  pmem_unmap(origpmemaddr, mapped_len);

  m=0;

  clock_gettime(CLOCK, &start);
  if ((pmemaddr = pmem_map_file(location, 0, 0,
				0666, &mapped_len, &is_pmem)) == NULL) {
    perror("pmem_map_file");
    fprintf(stderr, "Failed to pmem_map_file for filename:%s.\n", location);
    exit(-100);
  }
  origpmemaddr = pmemaddr;

  for (i=0;i<num_rows;i++){
    memcpy(line, pmemaddr, desired_line_len * size);
    pmemaddr += desired_line_len * size;
    m = seek_match(search_phrase, sp_len, line, desired_line_len);
    if (m==0){
      m_count++;
    }
  }
  pmem_unmap(origpmemaddr, mapped_len);
  clock_gettime(CLOCK, &end);
  elapsed_time_hr(start, end, "PMEM Fileparse");

  if(m_count != r_count || m_count == 0){
    printf("Mismatched counts, test did not pass\n");
  }

  unlink(location); // Use this to ensure the generated file is removed from the system upon finish
  free(location);
}
#endif

/*
 * Create a line of random characters
 * Line will be ll long and appears in l
 * Randomly, phrase contained in sp and of sp_len length will be added to l at a random position
 */
int create_line(char* sp, size_t sp_len, char* l, unsigned int ll){


  int i = 0;
  int r = 0;
  int flag = 0;

  for (i=0;i<ll;i++){
    r = (rand() % 128);
    while(!isalnum(r)){
      r = (rand() % 128);
    }
    l[i] = (char)r;
  }
  l[i+1] = '\0';

  r = rand() % 2;
  /* printf("R = %d\n", r); */
  if (r==0){
    flag = 0;
    r = rand() % (ll - sp_len);
    for (i=0;i<sp_len;i++){
      l[r+i] = sp[i];
    }
  }
  else{
    flag = 1;
  }

  return flag;
}

/*
 * Naive matching algorithm
 */
int seek_match(char* sp, size_t sp_len, char* l, unsigned int ll){

  int i = 0;
  int flag = 1;
  for (i=0;i<ll-sp_len;i++){
    if (l[i]==sp[0]){
      if (strncmp(&l[i], &sp[0], sp_len) == 0){
        flag = 0;
        break;
      }
    }
  }

  return flag;
}
