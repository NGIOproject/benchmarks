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
#include <time.h>
#include <limits.h>
#include <math.h>

#include "level0.h"
#include "utils.h"

/*
 *
 * Measure the impact of using a switch statement to
 * jump into different code blocks.
 *
 */
int switch_jump(unsigned long reps){

	int i;
	char op[] = "ABCD";
	volatile char o;

	struct timespec start, end;

	srand((unsigned int)time(NULL));

	/* Measure loop with 'nop' */
	clock_gettime(CLOCK, &start);
	for(i=0; i<reps; i++){
		__asm__("nop");
	}
	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "Loop without switch statement or % operation");

	/* Measure loop with 'nop' and % */
	clock_gettime(CLOCK, &start);
	for(i=0; i<reps; i++){
		o = op[i%4];
		__asm__("nop");
	}
	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "Loop without switch statement, but with % operation");

	/* Measure loop with switch where each case performs 'nop' */
	clock_gettime(CLOCK, &start);
	for(i=0; i<reps; i++){

		o = op[i%4];

		switch(o) {
		case 'A':
			__asm__ ("nop");
			break;
		case 'B':
			__asm__ ("nop");
			break;
		case 'C':
			__asm__ ("nop");
			break;
		case 'D':
			__asm__ ("nop");
			break;
		default:
			break;
		}
	}
	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "Loop with switch statement");

	return 0;

}

/*
 *
 * Measure the impact of using unconditional jumps
 * (i.e. goto) into differently code sections
 *
 */
int goto_jump(unsigned long reps){

	int i;
	static void *label[] = {&&A, &&B, &&C, &&D};

	struct timespec start, end;

	/* Measure loop with 'nop' */
	clock_gettime(CLOCK, &start);
	for(i=0; i<reps; i++){

		__asm__("nop");

	}
	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "Loop without GOTO statement");

	/* Measure loop with GOTO */
	clock_gettime(CLOCK, &start);
	for(i=0; i<reps; i++){

		goto *label[i%4];

		A: __asm__("nop");
		B: __asm__("nop");
		C: __asm__("nop");
		D: __asm__("nop");

	}
	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "Loop without GOTO statement");

	return 0;
}

/*
 *
 * Branching benchmark where the if-branch is always taken.
 * To try and avoid the most obvious compiler optimisations
 * by filling the loops with computation
 *
 */
int all_true(unsigned long reps){

	unsigned long i;
	unsigned long is_true = 0, is_false = 0, count = 0;
	unsigned long N = reps;

	double pi, exactpi;
	double nn = (double) N;

	struct timespec start, end;

	/* warm-up */
	for(i = 0; i < 100; i=i+10){
		double idx = (double) i;
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){
		count++;
		double idx = (double) i;
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "Loop without branches");

	pi = pi * 4.0/((double) N);
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi));

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){

		double idx = (double) i;

		if((i & 0) == 0){
			is_true++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
		else{
			is_false++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "All TRUE");

	pi = pi * 4.0/((double) N);
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi));

	printf("count = %ld\n", count);
	printf("is_true = %ld\n", is_true);
	printf("is_false = %ld\n", is_false);

	return 0;
}

/*
 *
 * Branching benchmark where the else-branch is always taken.
 * To try and avoid the most obvious compiler optimisations
 * by filling the loops with computation
 *
 */
int all_false(unsigned long reps){

	unsigned long i;
	unsigned long is_true = 0, is_false = 0, count = 0;

	double pi, exactpi;

	unsigned long N = reps;
	double nn = (double) N;

	struct timespec start, end;

	/* warm-up */
	for(i = 0; i < 100; i=i+10){
		double idx = (double) i;

		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){
		count++;
		double idx = (double) i;

		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "Loop without branches");

	pi = pi * 4.0/((double) N);
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi));

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){

		double idx = (double) i;

		if((i | 1) == 0){
			is_true++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
		else{
			is_false++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "All FALSE");

	pi = pi * 4.0/((double) N);
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi));

	printf("count = %ld\n", count);
	printf("is_true = %ld\n", is_true);
	printf("is_false = %ld\n", is_false);

	return 0;
}

/*
 *
 * Branching benchmark where the if and else-branches are taken alternately.
 * To try and avoid the most obvious compiler optimisations
 * by filling the loops with computation
 *
 */
int true_false(unsigned long reps){

	unsigned long i;
	unsigned long is_true = 0, is_false = 0, count = 0;

	double pi, exactpi;

	unsigned long N = reps;
	double nn = (double) N;

	struct timespec start, end;

	/* warm-up */
	for(i = 0; i < 100; i=i+10){
		double idx = (double) i;
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){
		count++;
		double idx = (double) i;
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "Loop without branches");

	pi = pi * 4.0/((double) N);
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi));

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){

		double idx = (double) i;

		if((i & 1) == 0){
			is_true++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
		else{
			is_false++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "TRUE-FALSE-TRUE-FALSE");

	pi = pi * 4.0/((double) N);
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi));

	printf("count = %ld\n", count);
	printf("is_true = %ld\n", is_true);
	printf("is_false = %ld\n", is_false);

	return 0;
}

/*
 *
 * Branching benchmark where the 2 if and 2 else-branches are taken alternately.
 * To try and avoid the most obvious compiler optimisations
 * by filling the loops with computation
 *
 */
int t2_f2(unsigned long reps){

	unsigned long i;
	unsigned long is_true = 0, is_false = 0, count = 0;

	double pi, exactpi;

	unsigned long N = reps;
	double nn = (double) N;

	struct timespec start, end;

	/* warm-up */
	for(i = 0; i < 100; i=i+10){
		double idx = (double) i;
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){
		count++;
		double idx = (double) i;
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "Loop without branches");

	pi = pi * 4.0/((double) N);
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi));

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){

		double idx = (double) i;

		if((i & 2) == 0){
			is_true++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
		else{
			is_false++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "2xTRUE 2xFALSE");

	pi = pi * 4.0/((double) N);
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi));

	printf("count = %ld\n", count);
	printf("is_true = %ld\n", is_true);
	printf("is_false = %ld\n", is_false);

	return 0;

}

/*
 *
 * Branching benchmark where the 4 if and 4 else-branches are taken alternately.
 * To try and avoid the most obvious compiler optimisations
 * by filling the loops with computation
 *
 */
int t4_f4(unsigned long reps){

	unsigned long i;
	unsigned long is_true = 0, is_false = 0, count = 0;

	double pi, exactpi;

	unsigned long N = reps;
	double nn = (double) N;

	struct timespec start, end;

	/* warm-up */
	for(i = 0; i < 100; i=i+10){
		double idx = (double) i;
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){
		count++;
		double idx = (double) i;
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "Loop without branches");

	pi = pi * 4.0/((double) N);
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi));

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){

		double idx = (double) i;

		if((i & 4) == 0){
			is_true++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
		else{
			is_false++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "4xTRUE 4xFALSE");

	pi = pi * 4.0/((double) N);
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi));

	printf("count = %ld\n", count);
	printf("is_true = %ld\n", is_true);
	printf("is_false = %ld\n", is_false);

	return 0;
}

/*
 *
 * Branching benchmark where the 8 if and 8 else-branches are taken alternately.
 * To try and avoid the most obvious compiler optimisations
 * by filling the loops with computation
 *
 */
int t8_f8(unsigned long reps){

	unsigned long i;
	unsigned long is_true = 0, is_false = 0, count = 0;;

	double pi, exactpi;

	unsigned long N = reps;
	double nn = (double) N;

	struct timespec start, end;

	/* warm-up */
	for(i = 0; i < 100; i=i+10){
		double idx = (double) i;
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){
		count++;
		double idx = (double) i;
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "Loop without branches");

	pi = pi * 4.0/((double) N);
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi));

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){
		double idx = (double) i;

		if((i & 8) == 0){
			is_true++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
		else{
			is_false++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "8xTRUE 8xFALSE");

	pi = pi * 4.0/((double) N);
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi));

	printf("count = %ld\n", count);
	printf("is_true = %ld\n", is_true);
	printf("is_false = %ld\n", is_false);

	return 0;
}

/*
 *
 * Branching benchmark where the if and else-branches are taken randomly.
 * To try and avoid the most obvious compiler optimisations
 * by filling the loops with computation
 *
 */
int t_f_random(unsigned long reps){

	unsigned long i, t;
	int j;
	unsigned long is_true = 0, is_false = 0, count = 0;

	double pi, exactpi;

	unsigned long N = reps;
	double nn = (double) N;

	srand((int)time(NULL));

	int *array = (int *)malloc(1000 * sizeof(int));

	if(array == NULL){
		printf("Out Of Memory: could not allocate space for the array.\n");
		return 0;
	}

	struct timespec start, end;

	for(i = 0; i < 1000; i++){
		array[i] = (int)rand()/(int)(RAND_MAX/100.0);
	}

	/* warm-up */
	for(i = 0; i < 100; i=i+10){
		double idx = (double) i;
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){
		t = array[i%1000];
		count++;
		double idx = (double) i;
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
		pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "Loop without branches");

	pi = t + (pi * 4.0/((double) N));
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", (pi - t), fabs(100.0*(pi-t-exactpi)/exactpi));

	pi = 0.0;

	clock_gettime(CLOCK, &start);

	for(i = 0; i < N; i=i+10){

		double idx = (double) i;

		t = array[i%1000];

		if((t % 2 ) == 0){
			is_true++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
		else{
			is_false++;
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn)   * (idx-0.5)/(nn));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+1) * (idx-0.5)/(nn+1));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+2) * (idx-0.5)/(nn+2));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+3) * (idx-0.5)/(nn+3));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+4) * (idx-0.5)/(nn+4));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+5) * (idx-0.5)/(nn+5));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+6) * (idx-0.5)/(nn+6));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+7) * (idx-0.5)/(nn+7));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+8) * (idx-0.5)/(nn+8));
			pi = pi + 1.0 / (1.0 + (idx-0.5)/(nn+9) * (idx-0.5)/(nn+9));
		}
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, "Random TRUE FALSE");

	pi = t + (pi * 4.0/((double) N));
	exactpi = 4.0*atan(1.0);

	/* verify result */
	printf("pi = %f, %% error = %f\n", (pi - t), fabs(100.0*(pi-t-exactpi)/exactpi));

	printf("count = %ld\n", count);
	printf("is_true = %ld\n", is_true);
	printf("is_false = %ld\n", is_false);

	return 0;
}

