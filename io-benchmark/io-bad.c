#define _GNU_SOURCE
#include "common.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void open(char *basename, const char *mode, FILE **f)
{
    char *name = NULL;
    int ret;
    ret = asprintf(&name, "%s", basename);
    if ( ret < 0 ) {
        fprintf(stderr, "[%d] Failed to allocate space for filename\n",
                get_rank());
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    *f = fopen(name, mode);
    if ( *f == NULL ) {
        fprintf(stderr, "[%d] Unable to open file %s\n", get_rank(), name);
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    free(name);
}

static void close(FILE *f)
{
    fclose(f);
}

static void read_header(int *val, FILE *f)
{
    int ret;
    ret = fread(val, sizeof(*val), 1, f);
    if ( ret != 1 ) {
        fprintf(stderr, "[%d] Failed reading header\n", get_rank());
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
}

static void write_header(int val, FILE *f)
{
    fwrite(&val, sizeof(val), 1, f);
    fsync(fileno(f));
}

static void read_input(T **data, int *nitems, FILE *f)
{
    int ret;
    read_header(nitems, f);
    alloc_data(data, *nitems);
    ret = fread(*data, sizeof(**data), *nitems, f);
    if ( ret != *nitems ) {
        fprintf(stderr, "[%d] Failed reading input\n", get_rank());
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
}

static void write_output(T *data, int nitems, FILE *f)
{
    write_header(nitems, f);
    fwrite(data, sizeof(*data), nitems, f);
    fsync(fileno(f));
}

int main(int argc, char **argv)
{
    FILE **output;
    FILE **input;
    T **data;
    int *nitems;
    char *directory = NULL;
    char *outname;
    char *inname = NULL;
    int fake_data;
    int timestep;
    int i;
    MPI_Init(&argc, &argv);

    WITH_TIMING(TOTAL,
                i = asprintf(&directory, "%s%d", argv[1], get_rank());
                if ( i < 0 ) {
                    fprintf(stderr, "[%d] Unable to allocate space for directory\n",
                            get_rank());
                    MPI_Abort(MPI_COMM_WORLD, -1);
                }
                if ( argc == 3 ) {
                    outname = argv[2];
                    fake_data = 1;
                } else if ( argc == 4 ) {
                    outname = argv[2];
                    inname = argv[3];
                    fake_data = 0;
                } else {
                    if ( !get_rank() ) {
                        fprintf(stderr, "Usage: %s DIRECTORY OUTFILE [INFILE]\n", basename(argv[0]));

                    }
                    MPI_Finalize();
                    return -1;
                }

                output = malloc(N_FILES * sizeof(*output));
                data = malloc(N_FILES * sizeof(*data));
                nitems = malloc(N_FILES * sizeof(*nitems));
                for ( timestep = 0; timestep < MAX_TIMESTEPS; timestep++ ) {
                    WITH_TIMING(ENSURE_DIRECTORY,
                                ensure_directory(directory, timestep));
                    WITH_TIMING(OPEN_OUTPUT,
                                for ( i = 0; i < N_FILES; i++ ) {
                                    char *fname = get_file_name(directory, outname, i, timestep);
                                    open(fname, "w", &(output[i]));
                                    free(fname);
                                });
                    if ( fake_data ) {
                        WITH_TIMING(FAKE_INPUT,
                                    for ( i = 0; i < N_FILES; i++ ) {
                                        fake_input(&(nitems[i]), i);
                                        alloc_data(&(data[i]), nitems[i]);
                                        init_data(data[i], nitems[i], get_rank());
                                    });
                    } else {
                        input = malloc(N_FILES * sizeof(*input));
                        WITH_TIMING(OPEN_INPUT,
                                    for ( i = 0; i < N_FILES; i++ ) {
                                        char *fname = get_file_name(directory, inname, i, timestep);
                                        open(fname, "r", &(input[i]));
                                        free(fname);
                                    });
                        WITH_TIMING(READ_INPUT,
                                    for ( i = 0; i < N_FILES; i++ ) {
                                        read_input(&(data[i]), &(nitems[i]), input[i]);
                                    });
                        WITH_TIMING(CLOSE_INPUT,
                                    for ( i = 0; i < N_FILES; i++ ) {
                                        close(input[i]);
                                    });
                        free(input);
                    }

                    WITH_TIMING(WRITE_OUTPUT,
                                for ( i = 0; i < N_FILES; i++ ) {
                                    write_output(data[i], nitems[i], output[i]);
                                });

                    WITH_TIMING(CLOSE_OUTPUT,
                                for ( i = 0; i < N_FILES; i++ ) {
                                    close(output[i]);
                                });
                    for ( i = 0; i < N_FILES; i++ ) {
                        dealloc_data(data[i]);
                    }
                }
                free(data);
                free(nitems);
                free(directory);
                free(output);
                MPI_Barrier(COMM);
        );
    for ( i = 0; i < INVALID_TIMING; i++ ) {
        PRINT_TIMING(i);
    }
    MPI_Finalize();

    return 0;
}
