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

#include "level1.h"
#include "utils.h"
#include "matrix_utils.h"

/*
 * Vector dot product, integers
 *
 * result = result + v1_i * v2_i
 *
 * Input: size of the vectors (in number of elements)
 * Output: dot product
 *
 */
int int_dot_product(unsigned int size) {

    int i;

    /* create two vectors */
    int *v1 = (int *) malloc(size * sizeof (int));
    int *v2 = (int *) malloc(size * sizeof (int));

    /* result variable */
    unsigned int result = 0;

    if (v1 == NULL || v2 == NULL) {
        printf("Out Of Memory: could not allocate space for the two arrays.\n");
        return 0;
    }

    srand((int) time(NULL));

    struct timespec start, end;

    /* fill vectors with random integer values */
    for (i = 0; i < size; i++) {
        v1[i] = (int) rand() / (int) (RAND_MAX / 10);
        v2[i] = (int) rand() / (int) (RAND_MAX / 10);
    }

    clock_gettime(CLOCK, &start);

    /* perform dot product */
    for (i = 0; i < size; i++) {
        result = result + v1[i] * v2[i];
    }

    clock_gettime(CLOCK, &end);

    /* print result so compiler does not throw it away */
    printf("Dot product result: %d\n", result);

    elapsed_time_hr(start, end, "Integer dot product.");

    free(v1);
    free(v2);

    return 0;

}

/*
 * Vector dot product, floats
 *
 * result = result + v1_i * v2_i
 *
 * Input: size of the vectors (in number of elements)
 * Output: dot product
 *
 */
int float_dot_product(unsigned int size) {

    int i;

    /* create three vectors */
    float *v1 = (float *) malloc(size * sizeof (float));
    float *v2 = (float *) malloc(size * sizeof (float));
    float result = 0.0;

    if (v1 == NULL || v2 == NULL) {
        printf("Out Of Memory: could not allocate space for the two arrays.\n");
        return 0;
    }

    srand((int) time(NULL));

    struct timespec start, end;

    /* fill vectors with random floats */
    for (i = 0; i < size; i++) {
        v1[i] = (float) rand() / (float) (RAND_MAX / 10.0);
        v2[i] = (float) rand() / (float) (RAND_MAX / 10.0);
    }

    clock_gettime(CLOCK, &start);

    /* perform dot product */
    for (i = 0; i < size; i++) {
        result = result + v1[i] * v2[i];
    }

    clock_gettime(CLOCK, &end);

    /* print result so compiler does not throw it away */
    printf("Dot product result: %f\n", result);

    elapsed_time_hr(start, end, "Float dot product.");

    free(v1);
    free(v2);

    return 0;

}

/*
 * Vector dot product, doubles
 *
 * result = result + v1_i * v2_i
 *
 * Input: size of the vectors (in number of elements)
 * Output: dot product
 *
 */
int double_dot_product(unsigned int size) {

    int i;

    /* create three vectors */
    double *v1 = (double *) malloc(size * sizeof (double));
    double *v2 = (double *) malloc(size * sizeof (double));
    double result = 0.0;

    if (v1 == NULL || v2 == NULL) {
        printf("Out Of Memory: could not allocate space for the two arrays.\n");
        return 0;
    }

    srand((int) time(NULL));

    struct timespec start, end;

    /* fill vectors with random floats */
    for (i = 0; i < size; i++) {
        v1[i] = (double) rand() / (double) (RAND_MAX / 10.0);
        v2[i] = (double) rand() / (double) (RAND_MAX / 10.0);
    }

    clock_gettime(CLOCK, &start);


    /* perform dot product */
    for (i = 0; i < size; i++) {
        result = result + v1[i] * v2[i];
    }
    clock_gettime(CLOCK, &end);

    /* print result so compiler does not throw it away */
    printf("Dot product result: %f\n", result);

    elapsed_time_hr(start, end, "Double dot product.");

    free(v1);
    free(v2);

    return 0;

}


/* Vector scalar product, integers    */

/* v_i = a * v1_i                     */
int int_scalar_mult(unsigned int size) {

    int i;

    /* create vector and scalar */
    int *v = (int *) malloc(size * sizeof (int));
    unsigned int a = 0;

    if (v == NULL) {
        printf("Out Of Memory: could not allocate space for the array.\n");
        return 0;
    }

    srand((int) time(NULL));

    struct timespec start, end;

    /* fill vector with random ints */
    for (i = 0; i < size; i++) {
        v[i] = (int) rand() / (int) (RAND_MAX / 10);
    }

    /* assign random int value */
    a = (int) rand() / (int) (RAND_MAX / 10);

    clock_gettime(CLOCK, &start);

    /* perform scalar product */
    for (i = 0; i < size; i++) {
        v[i] = a * v[i];
    }
    clock_gettime(CLOCK, &end);

    /* print result so compiler does not throw it away */
    printf("Scalar product result: %d\n", v[0]);

    elapsed_time_hr(start, end, "Int scalar multiplication.");

    free(v);

    return 0;

}

/* Vector scalar product, floats    */

/* v_i = a * v1_i                     */
int float_scalar_mult(unsigned int size) {

    int i;

    /* create vector and scalar */
    float *v = (float *) malloc(size * sizeof (float));
    float a = 0.0;

    if (v == NULL) {
        printf("Out Of Memory: could not allocate space for the array.\n");
        return 0;
    }

    srand((int) time(NULL));

    struct timespec start, end;

    /* fill vector with random floats */
    for (i = 0; i < size; i++) {
        v[i] = (float) rand() / (float) (RAND_MAX / 10.0);
    }

    /* assign random float value */
    a = (float) rand() / (float) (RAND_MAX / 10.0);

    clock_gettime(CLOCK, &start);

    /* perfom scalar product */
    for (i = 0; i < size; i++) {
        v[i] = a * v[i];
    }

    clock_gettime(CLOCK, &end);

    /* print result so compiler does not throw it away */
    printf("Scalar product result: %f\n", v[0]);

    elapsed_time_hr(start, end, "Float scalar multiplication.");

    free(v);

    return 0;

}

/* Vector scalar product, doubles    */

/* v_i = a * v1_i                     */
int double_scalar_mult(unsigned int size) {

    int i;

    /* create vector and scalar */
    double *v = (double *) malloc(size * sizeof (double));
    double a = 0.0;

    if (v == NULL) {
        printf("Out Of Memory: could not allocate space for the array.\n");
        return 0;
    }

    srand((int) time(NULL));

    struct timespec start, end;

    /* fill vector with random doubles */
    for (i = 0; i < size; i++) {
        v[i] = (double) rand() / (double) (RAND_MAX / 10.0);
    }

    /* assign random double value */
    a = (double) rand() / (double) (RAND_MAX / 10.0);

    clock_gettime(CLOCK, &start);

    /* perfom scalar product */
    for (i = 0; i < size; i++) {
        v[i] = a * v[i];
    }

    clock_gettime(CLOCK, &end);

    /* print result so compiler does not throw it away */
    printf("Scalar product result: %f\n", v[0]);

    elapsed_time_hr(start, end, "Double scalar multiplication.");

    free(v);

    return 0;

}

/* compute the Euclidean norm of an int vector      */
/* !!!! naive implementation -- find algorithm that  */

/* !!!! will avoid over/underflow for large vectors  */
int int_norm(unsigned int size) {

    int i;

    unsigned int *v = (unsigned int *) malloc(size * sizeof (unsigned int));
    unsigned int sum = 0;
    float norm = 0.0; /* Result is a float */

    if (v == NULL) {
        printf("Out Of Memory: could not allocate space for the array.\n");
        return 0;
    }

    struct timespec start, end;

    /* fill vector with random ints */
    for (i = 0; i < size; i++) {
        v[i] = 1 + (int) UNI;
    }

    clock_gettime(CLOCK, &start);

    for (i = 0; i < size; i++) {
        sum = sum + (v[i] * v[i]);
    }

    norm = sqrt(sum);

    clock_gettime(CLOCK, &end);

    elapsed_time_hr(start, end, "Int vector norm.");

    /* print result so compiler does not throw it away */
    printf("Norm = %f\n", norm);

    free(v);

    return 0;
}

/* compute the Euclidean norm of a float vector      */
/* !!!! naive implementation -- find algorithm that  */

/* !!!! will avoid over/underflow for large vectors  */
int float_norm(unsigned int size) {

    int i;

    float *v = (float *) malloc(size * sizeof (float));
    float sum = 0.0, norm = 0.0;

    if (v == NULL) {
        printf("Out Of Memory: could not allocate space for the array.\n");
        return 0;
    }

    srand((int) time(NULL));

    struct timespec start, end;

    /* fill vector with random floats */
    for (i = 0; i < size; i++) {
        v[i] = (float) rand() / (float) (RAND_MAX / 10.0);
    }

    clock_gettime(CLOCK, &start);

    for (i = 0; i < size; i++) {
        sum = sum + (v[i] * v[i]);
    }

    norm = sqrt(sum);

    clock_gettime(CLOCK, &end);

    elapsed_time_hr(start, end, "Float vector norm.");

    /* print result so compiler does not throw it away */
    printf("Norm = %f\n", norm);

    free(v);

    return 0;
}

/* compute the Euclidean norm of a double vector      */
/* !!!! naive implementation -- find algorithm that  */

/* !!!! will avoid over/underflow for large vectors  */
int double_norm(unsigned int size) {

    int i;

    double *v = (double *) malloc(size * sizeof (double));
    double sum = 0.0, norm = 0.0;

    if (v == NULL) {
        printf("Out Of Memory: could not allocate space for the array.\n");
        return 0;
    }

    struct timespec start, end;

    /* fill vector with random doubles */
    for (i = 0; i < size; i++) {
        v[i] = UNI;
    }

    clock_gettime(CLOCK, &start);

    for (i = 0; i < size; i++) {
        sum = sum + (v[i] * v[i]);
    }

    norm = sqrt(sum);

    clock_gettime(CLOCK, &end);

    elapsed_time_hr(start, end, "Double vector norm.");

    /* print result so compiler does not throw it away */
    printf("Norm = %f\n", norm);

    free(v);

    return 0;
}

/*
 *
 * Compute vector-scalar product
 * AXPY, integers
 *
 * y = a * x + y
 *
 * Naive implementation
 *
 */
int int_axpy(unsigned int size) {

    int i, a;
    int *x = (int *) malloc(size * sizeof (int));
    int *y = (int *) malloc(size * sizeof (int));

    if (x == NULL || y == NULL) {
        printf("Out Of Memory: could not allocate space for the two arrays.\n");
        return 0;
    }

    srand((int) time(NULL));

    a = (int) rand() / (int) (RAND_MAX / 10);

    /* fill x and y vectors with random ints */
    for (i = 0; i < size; i++) {
        x[i] = (int) rand() / (int) (RAND_MAX / 10);
        y[i] = (int) rand() / (int) (RAND_MAX / 10);
    }

    struct timespec start, end;

    clock_gettime(CLOCK, &start);

    for (i = 0; i < size; i++) {
        y[i] = a * x[i] + y[i];
    }

    clock_gettime(CLOCK, &end);

    elapsed_time_hr(start, end, "Int AXPY.");

    /* print some of the result so compiler does not throw it away */
    printf("APXY result = %d\n", y[0]);

    free(x);
    free(y);

    return 0;
}

/*
 *
 * Compute vector-scalar product
 * AXPY, floats
 *
 * y = a * x + y
 *
 * Naive implementation
 *
 */
int float_axpy(unsigned int size) {

    int i;
    float a;
    float *x = (float *) malloc(size * sizeof (float));
    float *y = (float *) malloc(size * sizeof (float));

    if (x == NULL || y == NULL) {
        printf("Out Of Memory: could not allocate space for the two arrays.\n");
        return 0;
    }

    srand((int) time(NULL));

    a = (float) rand() / (float) (RAND_MAX / 10.0);

    /* fill x and y vectors with random ints */
    for (i = 0; i < size; i++) {
        x[i] = (float) rand() / (float) (RAND_MAX / 10.0);
        y[i] = (float) rand() / (float) (RAND_MAX / 10.0);
    }

    struct timespec start, end;

    clock_gettime(CLOCK, &start);

    for (i = 0; i < size; i++) {
        y[i] = a * x[i] + y[i];
    }

    clock_gettime(CLOCK, &end);

    elapsed_time_hr(start, end, "Float AXPY.");

    /* print some of the result so compiler does not throw it away */
    printf("APXY result = %f\n", y[0]);

    free(x);
    free(y);

    return 0;
}

/*
 *
 * Compute vector-scalar product
 * AXPY, doubles
 *
 * y = a * x + y
 *
 * Naive implementation
 *
 */
int double_axpy(unsigned int size) {

    int i;
    double a;
    double *x = (double *) malloc(size * sizeof (double));
    double *y = (double *) malloc(size * sizeof (double));

    if (x == NULL || y == NULL) {
        printf("Out Of Memory: could not allocate space for the two arrays.\n");
        return 0;
    }

    srand((int) time(NULL));

    a = (double) rand() / (double) (RAND_MAX / 10.0);

    /* fill x and y vectors with random ints */
    for (i = 0; i < size; i++) {
        x[i] = (double) rand() / (double) (RAND_MAX / 10.0);
        y[i] = (double) rand() / (double) (RAND_MAX / 10.0);
    }

    struct timespec start, end;

    clock_gettime(CLOCK, &start);

    for (i = 0; i < size; i++) {
        y[i] = a * x[i] + y[i];
    }

    clock_gettime(CLOCK, &end);

    elapsed_time_hr(start, end, "Double AXPY.");

    /* print some of the result so compiler does not throw it away */
    printf("APXY result = %f\n", y[0]);

    free(x);
    free(y);

    return 0;
}

/*
 * Dense Matrix-Vector product, integers
 *
 * y = A * x
 * where A is a square matrix
 *
 * Input:  number of elements in vectors and of rows/cols
 *         in matrix specified as number of ints
 *
 */
int int_dmatvec_product(unsigned int size) {

    int i, j;
    int r1, r2;

    /* create two vectors */
    int *x = (int *) malloc(size * sizeof (int));
    int *y = (int *) calloc(size, sizeof (int));

    /* create matrix */
    int **A;
    A = (int **) malloc(size * sizeof (int *));
    for (i = 0; i < size; i++) {
        A[i] = (int *) malloc(size * sizeof (int));
    }

    if (x == NULL || y == NULL || A == NULL) {
        printf("Out Of Memory: could not allocate space for the vectors and matrix.\n");
        return 0;
    }

    srand((int) time(NULL));

    struct timespec start, end;

    r1 = (int) rand() / (int) (RAND_MAX / 10);
    r2 = (int) rand() / (int) (RAND_MAX / 10);

    /* fill vector x and matrix A with random integer values */
    for (i = 0; i < size; i++) {
        x[i] = r1;
        for (j = 0; j < size; j++) {
            A[i][j] = r2;
        }
    }

    clock_gettime(CLOCK, &start);

    /* perform matrix-vector product */
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            y[i] = y[i] + A[i][j] * x[j];
        }
    }

    clock_gettime(CLOCK, &end);

    elapsed_time_hr(start, end, "Dense Matrix-Vector product.");

    /* print result so compiler does not throw it away */
    printf("Result vector y[0] = %d\n", y[0]);


    free(x);
    free(y);
    for (i = 0; i < size; i++) free(A[i]);
    free(A);

    return 0;

}

/*
 * Dense Matrix-Vector product, floats
 *
 * y = A * x
 * where A is a square matrix
 *
 * Input:  number of elements in vectors and of rows/cols
 *         in matrix specified as number of floats
 *
 */
int float_dmatvec_product(unsigned int size) {

    int i, j;
    float r1, r2;

    /* create two vectors */
    float *x = (float *) malloc(size * sizeof (float));
    float *y = (float *) calloc(size, sizeof (float));

    /* create matrix */
    float **A;
    A = (float **) malloc(size * sizeof (float *));
    for (i = 0; i < size; i++) {
        A[i] = (float *) malloc(size * sizeof (float));
    }

    if (x == NULL || y == NULL || A == NULL) {
        printf("Out Of Memory: could not allocate space for the vectors and matrix.\n");
        return 0;
    }

    srand((int) time(NULL));

    struct timespec start, end;

    r1 = (float) rand() / (float) (RAND_MAX / 10.0);
    r2 = (float) rand() / (float) (RAND_MAX / 10.0);

    /* fill vector x and matrix A with random integer values */
    for (i = 0; i < size; i++) {
        x[i] = r1;
        for (j = 0; j < size; j++) {
            A[i][j] = r2;
        }
    }

    clock_gettime(CLOCK, &start);

    /* perform matrix-vector product */
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            y[i] = y[i] + A[i][j] * x[j];
        }
    }

    clock_gettime(CLOCK, &end);

    elapsed_time_hr(start, end, "Dense Matrix-Vector product.");

    /* print result so compiler does not throw it away */
    printf("Result vector y[0] = %f\n", y[0]);


    free(x);
    free(y);
    for (i = 0; i < size; i++) free(A[i]);
    free(A);

    return 0;

}

/*
 * Dense Matrix-Vector product, doubles
 *
 * y = A * x
 * where A is a square matrix
 *
 * Input:  number of elements in vectors and of rows/cols
 *         in matrix specified as number of floats
 *
 */
int double_dmatvec_product(unsigned int size) {

    int i, j;
    double r1, r2;

    /* create two vectors */
    double *x = (double *) malloc(size * sizeof (double));
    double *y = (double *) calloc(size, sizeof (double));

    if (x == NULL || y == NULL) {
        printf("Out Of Memory: could not allocate space for the vectors.\n");
        return 0;
    }


    /* create matrix */
    double **A;
    A = (double **) malloc(size * sizeof (double *));
    if (A == NULL) {
        printf("Out Of Memory: could not allocate space for the matrix.\n");
        return 0;
    }

    for (i = 0; i < size; i++) {
        A[i] = (double *) malloc(size * sizeof (double));
        if (A[i] == NULL) {
            printf("Out Of Memory: could not allocate space for the matrix.\n");
            return 0;
        }
    }

    srand((int) time(NULL));

    struct timespec start, end;

    r1 = (double) rand() / (double) (RAND_MAX / 10.0);
    r2 = (double) rand() / (double) (RAND_MAX / 10.0);

    /* fill vector x and matrix A with random integer values */
    for (i = 0; i < size; i++) {
        x[i] = r1;
        for (j = 0; j < size; j++) {
            A[i][j] = r2;
        }
    }

    clock_gettime(CLOCK, &start);

    /* perform matrix-vector product */
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            y[i] = y[i] + A[i][j] * x[j];
        }
    }

    clock_gettime(CLOCK, &end);

    elapsed_time_hr(start, end, "Dense Matrix-Vector product.");

    /* print result so compiler does not throw it away */
    printf("Result vector y[0] = %f\n", y[0]);


    free(x);
    free(y);
    for (i = 0; i < size; i++) free(A[i]);
    free(A);

    return 0;

}

int float_spmatvec_product(unsigned long r) {

    int m, n, nz;
    int *row_idx, *col_idx;
    float *values;
    float *x, *b;

    struct timespec start, end;

    char *filename = "matrix_sml.csr";

    int i, j, rep;

    FILE *f;
    char line[64];
    char *rv = NULL;

    if (r == ULONG_MAX) r = 1000;

    if ((f = fopen(filename, "r")) == NULL) {
        printf("can't open file <%s> \n", filename);
        exit(1);
    }

    rv = fgets(line, sizeof (line), f);
    if (*rv != EOF) {
        sscanf(line, "%d %d %d", &nz, &n, &m);
    } else {
        printf("Failed to read file\n");
        exit(1);
    }

    printf("Number of elements of values and col_idx: %d; number of values in row_idx: %d\n", nz, m);

    row_idx = malloc(m * sizeof (int));
    col_idx = malloc(n * sizeof (int));
    values = malloc(nz * sizeof (float));
    x = malloc((m - 1) * sizeof (float));
    b = malloc((m - 1) * sizeof (float));

    if (!row_idx || !col_idx || !values || !x || !b) {
        printf("cannot allocate memory for sparse matrix and vectors\n");
        exit(1);
    } else {
        printf("memory allocated\n");
    }


    // fill values
    rv = NULL;
    for (i = 0; i < nz; i++) {
        rv = fgets(line, sizeof (line), f);
        if (*rv != EOF) {
            sscanf(line, "%f", &values[i]);
        } else {
            printf("Failed to read file\n");
            exit(1);
        }
    }

    rv = NULL;
    // fill col_idx
    for (i = 0; i < nz; i++) {
        rv = fgets(line, sizeof (line), f);
        if (*rv != EOF) {
            sscanf(line, "%d", &col_idx[i]);
        } else {
            printf("Failed to read file\n");
            exit(1);
        }

    }


    rv = NULL;
    // fill row_idx
    for (i = 0; i < m; i++) {
        rv = fgets(line, sizeof (line), f);
        if (*rv != EOF) {
            sscanf(line, "%d", &row_idx[i]);
        } else {
            printf("Failed to read file\n");
            exit(1);
        }
    }

    for (i = 0; i < m - 1; i++) {
        x[i] = i + 1.5; // give basic values to vector x
    }

    clock_gettime(CLOCK, &start);

    for (rep = 0; rep < r; rep++) {
        /* Ax=b */
        for (i = 0; i < m - 1; i++) {
            for (j = row_idx[i]; j < row_idx[i + 1]; j++) {
                b[i] = b[i] + values[j] * x[col_idx[j]];
            }
        }
    }

    clock_gettime(CLOCK, &end);

    elapsed_time_hr(start, end, "Sparse DMVs.");

    free(x);
    free(b);
    free(row_idx);
    free(col_idx);
    free(values);

    fclose(f);

    return 0;
}

int double_spmatvec_product(unsigned long r) {

    //int *m, *n, *nz;
    int m, n, nz;
    int *row_idx, *col_idx;
    double *values;
    double *x, *b;

    struct timespec start, end;

    char *filename = "matrix_sml.csr";

    int i, j, rep;

    FILE *f;
    char line[64];
    char *rv = NULL;

    if (r == ULONG_MAX) r = 1000;

    if ((f = fopen(filename, "r")) == NULL) {
        printf("can't open file <%s> \n", filename);
        exit(1);
    }
    rv = fgets(line, sizeof (line), f);
    if (*rv != EOF) {
        sscanf(line, "%d %d %d", &nz, &n, &m);
    } else {
        printf("Failed to read file\n");
        exit(1);
    }

    /* fgets(line, sizeof(line), f); */
    /* sscanf(line, "%d %d %d", &nz, &n, &m); */

    printf("Number of elements of values and col_idx: %d; number of values in row_idx: %d\n", nz, m);

    row_idx = malloc(m * sizeof (int));
    col_idx = malloc(n * sizeof (int));
    values = malloc(nz * sizeof (double));
    x = malloc((m - 1) * sizeof (double));
    b = malloc((m - 1) * sizeof (double));

    if (!row_idx || !col_idx || !values || !x || !b) {
        printf("cannot allocate memory for sparse matrix and vectors\n");
        exit(1);
    } else {
        printf("memory allocated\n");
    }

    // fill values
    rv = NULL;
    for (i = 0; i < nz; i++) {
        rv = fgets(line, sizeof (line), f);
        if (*rv != EOF) {
            sscanf(line, "%lf", &values[i]);
        } else {
            printf("Failed to read file\n");
            exit(1);
        }
    }

    rv = NULL;
    // fill col_idx
    for (i = 0; i < nz; i++) {
        rv = fgets(line, sizeof (line), f);
        if (*rv != EOF) {
            sscanf(line, "%d", &col_idx[i]);
        } else {
            printf("Failed to read file\n");
            exit(1);
        }

    }


    rv = NULL;
    // fill row_idx
    for (i = 0; i < m; i++) {
        rv = fgets(line, sizeof (line), f);
        if (*rv != EOF) {
            sscanf(line, "%d", &row_idx[i]);
        } else {
            printf("Failed to read file\n");
            exit(1);
        }
    }


    /* // fill col_idx */
    /* for(i=0;i<nz;i++){ */
    /*   fgets(line, sizeof(line), f); */
    /*   sscanf(line, "%d", &col_idx[i]); */
    /* } */

    /* // fill row_idx */
    /* for(i=0;i<m;i++){ */
    /*   fgets(line, sizeof(line), f); */
    /*   sscanf(line, "%d", &row_idx[i]); */
    /* } */

    for (i = 0; i < m - 1; i++) {
        x[i] = i + 1.5; // give basic values to vector x
    }

    clock_gettime(CLOCK, &start);

    for (rep = 0; rep < r; rep++) {
        /* Ax=b */
        for (i = 0; i < m - 1; i++) {
            for (j = row_idx[i]; j < row_idx[i + 1]; j++) {
                b[i] = b[i] + values[j] * x[col_idx[j]];
            }
        }
    }

    clock_gettime(CLOCK, &end);

    elapsed_time_hr(start, end, "Sparse DMVs.");

    free(x);
    free(b);
    free(row_idx);
    free(col_idx);
    free(values);

    fclose(f);

    return 0;
}

int float_spgemm(unsigned long r) {

    int m, n, nz;
    int *row_csr_idx, *col_csr_idx;
    int *row_csc_idx, *col_csc_idx;
    float *A_csr, *B_csc, *C; // matrices
    double *tmp;
    
    /* Change this to matrix_lrg.csr and recompile to use the large matrix */
    char *filename = "matrix_sml.csr";
    char line[64];
    char *rv = NULL;

    int i, j, k;
    unsigned long rep = 0;
    int count = 0, nz_count = 0;

    struct timespec start, end;

    if (r == ULONG_MAX) r = 100;

    FILE *f;
    if ((f = fopen(filename, "r")) == NULL) {
        printf("can't open file <%s> \n", filename);
        exit(1);
    }
    rv = fgets(line, sizeof (line), f);
    if (*rv != EOF) {
        sscanf(line, "%d %d %d", &nz, &n, &m);
    } else {
        printf("Failed to read file\n");
        exit(1);
    }



    row_csr_idx = malloc(nz * sizeof (int));
    col_csr_idx = malloc(nz * sizeof (int));
    A_csr = malloc(nz * sizeof (float));
    tmp = malloc(nz * sizeof (double));
    row_csc_idx = malloc(nz * sizeof (int));
    col_csc_idx = malloc(nz * sizeof (int));
    B_csc = malloc(nz * sizeof (float));
    C = calloc((m * n), sizeof (float));

    if (!row_csr_idx || !col_csr_idx || !A_csr || !row_csc_idx || !col_csc_idx || !B_csc || !C) {
        printf("cannot allocate memory for %d, %d, %d sparse matrices and vector\n", m, n, nz);
        exit(1);
    } else {
        printf("memory allocated\n");
    }

    clock_gettime(CLOCK, &start);

    // fill A_csr
    for (i = 0; i < nz; i++) {
        rv = fgets(line, sizeof (line), f);
        k = sscanf(line, "%f", &A_csr[i]);

    }

    // fill col_csr_idx
    for (i = 0; i < nz; i++) {
        rv = fgets(line, sizeof (line), f);
        sscanf(line, "%d", &col_csr_idx[i]);
    }

    // fill row_csr_idx
    for (i = 0; i < m; i++) {
        rv = fgets(line, sizeof (line), f);
        sscanf(line, "%d", &row_csr_idx[i]);
    }

    fclose(f);
    clock_gettime(CLOCK, &end);
    elapsed_time_hr(start, end, "Read in CSR file");




    m -= 1;
    n = m;

    col_csc_idx[0] = 0;

    /* create B_csc from A_csr */
    for (i = 0; i < m; i++) {

        nz_count = 0;

        for (j = 0; j < m; j++) {

            for (k = row_csr_idx[j]; k < row_csr_idx[j + 1]; k++) {

                if (col_csr_idx[k] == i) {
                    B_csc[count] = A_csr[nz_count];
                    row_csc_idx[count] = j;
                    //      printf("j = %d and k = %d\n", j, k);
                    count++;
                }
                nz_count++;
                //      printf("nz = %d\n", nz_count);
            }
            col_csc_idx[i + 1] = count;
        }
    }

    clock_gettime(CLOCK, &start);

    for (rep = 0; rep < r; rep++) {

        /* A*B=C */

        for (j = 0; j < n; j++) { // cols

            /* temporary padded column vector */
            float *temp_vec = malloc(nz * sizeof (float));

            for (k = col_csc_idx[j]; k < col_csc_idx[j + 1]; k++) {
                temp_vec[row_csc_idx[k]] = B_csc[k];
            }

            /* spgemm */
            for (i = 0; i < m; i++) { // rows

                for (k = row_csr_idx[i]; k < row_csr_idx[i + 1]; k++) {
                    C[j + i * m] = C[j + i * m] + A_csr[k] * temp_vec[col_csr_idx[k]];
                }

            }

            /* free temporary vector */
            free(temp_vec);

        }

    }

    clock_gettime(CLOCK, &end);

    elapsed_time_hr(start, end, "Sparse float GEMM.");

    /* free memory*/
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

int double_spgemm(unsigned long r) {

    int m, n, nz;
    int *row_csr_idx, *col_csr_idx;
    int *row_csc_idx, *col_csc_idx;
    double *A_csr, *B_csc, *C; // matrices

    /* Change this to matrix_lrg.csr and recompile to use the large matrix */
    char *filename = "matrix_sml.csr";
    char line[64];
    char *rv = NULL;

    int i, j, k;
    unsigned long rep = 0;
    int count = 0, nz_count = 0;

    struct timespec start, end;

    if (r == ULONG_MAX) r = 100;

    FILE *f;
    if ((f = fopen(filename, "r")) == NULL) {
        printf("can't open file <%s> \n", filename);
        exit(1);
    }
    rv = fgets(line, sizeof (line), f);
    if (*rv != EOF) {
        sscanf(line, "%d %d %d", &nz, &n, &m);
    } else {
        printf("Failed to read file\n");
        exit(1);
    }


    row_csr_idx = malloc(nz * sizeof (int));
    col_csr_idx = malloc(nz * sizeof (int));
    A_csr = malloc(nz * sizeof (double));
    row_csc_idx = malloc(nz * sizeof (int));
    col_csc_idx = malloc(nz * sizeof (int));
    B_csc = malloc(nz * sizeof (double));
    C = calloc((m * n), sizeof (double));

    if (!row_csr_idx || !col_csr_idx || !A_csr || !row_csc_idx || !col_csc_idx || !B_csc || !C) {
        printf("cannot allocate memory for %d, %d, %d sparse matrices and vector\n", m, n, nz);
        exit(1);
    }

    clock_gettime(CLOCK, &start);

    // fill A_csr
    for (i = 0; i < nz; i++) {
        rv = fgets(line, sizeof (line), f);
        k = sscanf(line, "%lf", &A_csr[i]);

    }

    // fill col_csr_idx
    for (i = 0; i < nz; i++) {
        rv = fgets(line, sizeof (line), f);
        sscanf(line, "%d", &col_csr_idx[i]);
    }

    // fill row_csr_idx
    for (i = 0; i < m; i++) {
        rv = fgets(line, sizeof (line), f);
        sscanf(line, "%d", &row_csr_idx[i]);
    }

    fclose(f);
    clock_gettime(CLOCK, &end);
    elapsed_time_hr(start, end, "Read in CSR file");

    m -= 1;
    n = m;

    col_csc_idx[0] = 0;

    /* create B_csc from A_csr */
    for (i = 0; i < m; i++) {

        nz_count = 0;

        for (j = 0; j < m; j++) {

            for (k = row_csr_idx[j]; k < row_csr_idx[j + 1]; k++) {

                if (col_csr_idx[k] == i) {
                    B_csc[count] = A_csr[nz_count];
                    row_csc_idx[count] = j;
                    //      printf("j = %d and k = %d\n", j, k);
                    count++;
                }
                nz_count++;
                //      printf("nz = %d\n", nz_count);
            }
            col_csc_idx[i + 1] = count;
        }
    }

    clock_gettime(CLOCK, &start);

    for (rep = 0; rep < r; rep++) {

        /* AB=C */

        for (j = 0; j < n; j++) { // cols

            /* temporary padded column vector */
            double *temp_vec = malloc(nz * sizeof (double));

            for (k = col_csc_idx[j]; k < col_csc_idx[j + 1]; k++) {
                temp_vec[row_csc_idx[k]] = B_csc[k];
            }

            /* spgemm */
            for (i = 0; i < m; i++) { // rows

                for (k = row_csr_idx[i]; k < row_csr_idx[i + 1]; k++) {
                    C[j + i * m] = C[j + i * m] + A_csr[k] * temp_vec[col_csr_idx[k]];
                }

            }

            /* free temporary vector */
            free(temp_vec);

        }

    }

    clock_gettime(CLOCK, &end);

    elapsed_time_hr(start, end, "Sparse DGEMMs");

    /* free memory*/
    free(row_csr_idx);
    free(col_csr_idx);
    free(row_csc_idx);
    free(col_csc_idx);
    free(A_csr);
    free(B_csc);
    free(C);

    return 0;
}
