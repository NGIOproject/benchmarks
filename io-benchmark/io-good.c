#define _GNU_SOURCE
#include "common.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static MPI_Datatype data_view[N_FILES] = {MPI_DATATYPE_NULL,
                                          MPI_DATATYPE_NULL,
                                          MPI_DATATYPE_NULL,
                                          MPI_DATATYPE_NULL,
                                          MPI_DATATYPE_NULL,
                                          MPI_DATATYPE_NULL};
static MPI_Datatype header_view = MPI_DATATYPE_NULL;

static void make_header_view()
{
    int size;
    int rank;
    int lsize;
    int start;
    if ( header_view != MPI_DATATYPE_NULL ) {
        return;
    }
    size = get_size();
    rank = get_rank();
    ++size;                     /* Proc 0 writes two values */
    if ( rank == 0 ) {
        lsize = 2;
        start = 0;
    } else {
        lsize = 1;
        start = rank + 1;
    }
    MPI_Type_create_subarray(1, &size, &lsize, &start,
                             MPI_ORDER_C, MPI_INT, &header_view);
    MPI_Type_commit(&header_view);
}

static void make_data_view(int nval, int which)
{
    int gval;
    int start = 0;
    if ( data_view[which] != MPI_DATATYPE_NULL ) {
        return;
    }
    MPI_Allreduce(&nval, &gval, 1, MPI_INT, MPI_SUM, COMM);
    MPI_Exscan(&nval, &start, 1, MPI_INT, MPI_SUM, COMM);
    MPI_Type_create_subarray(1, &gval, &nval, &start,
                             MPI_ORDER_C, DATA_MPI_TYPE, &(data_view[which]));
    MPI_Type_commit(&(data_view[which]));
}

static void free_views(void)
{
    int i = 0;
    if ( header_view != MPI_DATATYPE_NULL ) {
        MPI_Type_free(&header_view);
        header_view = MPI_DATATYPE_NULL;
    }
    for ( i = 0; i < N_FILES; i++ ) {
        if ( data_view[i] != MPI_DATATYPE_NULL ) {
            MPI_Type_free(&(data_view[i]));
        }
        data_view[i] = MPI_DATATYPE_NULL;
    }
}

static MPI_Offset read_header(int *val, MPI_File fh)
{
    int rank;
    int size;
    MPI_Status s;
    int nval = 1;
    int buf[2];
    make_header_view();
    rank = get_rank();
    size = get_size();
    if ( rank == 0 ) {
        nval = 2;
    }
    MPI_File_set_view(fh, (MPI_Offset)0, MPI_INT, header_view, "native",
                      MPI_INFO_NULL);
    MPI_File_read_all(fh, rank ? val : buf, nval, MPI_INT, &s);

    if ( rank == 0 && buf[0] != size ) {
        fprintf(stderr,
                "Data was decomposed for %d processes, but have %d, aborting\n",
                buf[0], size);
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    if ( rank == 0 ) {
        *val = buf[1];
    }
    return (MPI_Offset)((1 + size) * sizeof(int));
}

static MPI_Offset write_header(int val, MPI_File fh)
{
    int rank;
    int size;
    MPI_Status s;
    int val0[2] = {val, val};
    int nitems = 1;
    rank = get_rank();
    size = get_size();

    make_header_view();

    if ( rank == 0 ) {
        val0[0] = size;
        nitems = 2;
    }
    MPI_File_set_view(fh, (MPI_Offset)0, MPI_INT, header_view, "native",
                      MPI_INFO_NULL);
    MPI_File_write_all(fh, val0, nitems, MPI_INT, &s);

    return (MPI_Offset)((1 + size) * sizeof(int));
}

static void open(char *name, int mode, MPI_File *fh)
{
    int ierr;
    ierr = MPI_File_open(COMM, name,
                         mode, MPI_INFO_NULL, fh);
    if ( ierr ) {
        if ( !get_rank() ) {
            fprintf(stderr, "Unable to open file %s\n", name);
        }
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
}

static void close(MPI_File *fh)
{
    MPI_File_close(fh);
}

static void read_input(T **data, int *nitems, int which, MPI_File fh)
{
    MPI_Status s;
    MPI_Offset offset;
    offset = read_header(nitems, fh);
    alloc_data(data, *nitems);

    make_data_view(*nitems, which);
    MPI_File_set_view(fh, offset, DATA_MPI_TYPE, data_view[which], "native",
                      MPI_INFO_NULL);
    MPI_File_read_all(fh, *data, *nitems, DATA_MPI_TYPE, &s);
}

static void write_output(T *data, int nitems, int which, MPI_File fh)
{
    MPI_Status s;
    MPI_Offset offset;
    /* Truncate */
    MPI_File_set_size(fh, (MPI_Offset)0);
    offset = write_header(nitems, fh);
    make_data_view(nitems, which);
    MPI_File_set_view(fh, offset, DATA_MPI_TYPE, data_view[which], "native",
                      MPI_INFO_NULL);
    MPI_File_write_all(fh, data, nitems, DATA_MPI_TYPE, &s);
}

int main(int argc, char **argv)
{
    MPI_File *output;
    MPI_File *input;
    T **data;
    int *nitems;
    char *directory;
    char *outname;
    char *inname = NULL;
    int fake_data;
    int i;
    int timestep;
    MPI_Init(&argc, &argv);

    WITH_TIMING(TOTAL,
                if ( argc == 3 ) {
                    directory = argv[1];
                    outname = argv[2];
                    fake_data = 1;
                } else if ( argc == 4 ) {
                    directory = argv[1];
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
                                if ( !get_rank() ) {
                                    ensure_directory(directory, timestep);
                                }
                                MPI_Barrier(COMM));
                    WITH_TIMING(OPEN_OUTPUT,
                                for ( i = 0; i < N_FILES; i++ ) {
                                    char *fname = get_file_name(directory, outname, i, timestep);
                                    open(fname, MPI_MODE_WRONLY | MPI_MODE_CREATE,
                                         &(output[i]));
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
                                        open(fname, MPI_MODE_RDONLY, &(input[i]));
                                        free(fname);
                                    });
                        WITH_TIMING(READ_INPUT,
                                    for ( i = 0; i < N_FILES; i++ ) {
                                        read_input(&(data[i]), &(nitems[i]), i, input[i]);
                                    });
                        WITH_TIMING(CLOSE_INPUT,
                                    for ( i = 0; i < N_FILES; i++ ) {
                                        close(&(input[i]));
                                    });
                        free(input);
                    }
                    WITH_TIMING(WRITE_OUTPUT,
                                for ( i = 0; i < N_FILES; i++ ) {
                                    write_output(data[i], nitems[i], i, output[i]);
                                });

                    WITH_TIMING(CLOSE_OUTPUT,
                                for ( i = 0; i < N_FILES; i++ ) {
                                    close(&(output[i]));
                                });

                    for ( i = 0; i < N_FILES; i++ ) {
                        dealloc_data(data[i]);
                    }
                }
                free(data);
                free(nitems);
                free(output);
                free_views();
                MPI_Barrier(COMM);
        );
    for ( i = 0; i < INVALID_TIMING; i++ ) {
        PRINT_TIMING(i);
    }

    MPI_Finalize();

    return 0;
}
