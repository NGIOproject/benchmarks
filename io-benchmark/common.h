#ifndef _COMMON_H
#define _COMMON_H

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <error.h>

typedef int T;

#define DATA_MPI_TYPE MPI_INT

static MPI_Comm COMM = MPI_COMM_WORLD;

#define MAX_TIMESTEPS 5
#define ITEM(x) [x] = #x
enum timing_type { OPEN_OUTPUT,
                   FAKE_INPUT,
                   OPEN_INPUT,
                   READ_INPUT,
                   CLOSE_INPUT,
                   WRITE_OUTPUT,
                   CLOSE_OUTPUT,
                   ENSURE_DIRECTORY,
                   TOTAL,
                   INVALID_TIMING };

static char *timing_str[] = {
    ITEM(OPEN_OUTPUT),
    ITEM(OPEN_INPUT),
    ITEM(CLOSE_OUTPUT),
    ITEM(CLOSE_INPUT),
    ITEM(READ_INPUT),
    ITEM(FAKE_INPUT),
    ITEM(WRITE_OUTPUT),
    ITEM(ENSURE_DIRECTORY),
    ITEM(TOTAL)
};

enum stat_type { MIN, MAX, MEAN, INVALID_STAT };

static double timings[INVALID_TIMING][INVALID_STAT];


#define WITH_TIMING(name, block) do {                   \
        double start_##name = MPI_Wtime();              \
        double end_##name;                              \
        do {                                            \
            block;                                      \
        } while (0);                                    \
        end_##name = MPI_Wtime() - start_##name;        \
        timings[name][MIN] += end_##name;               \
        timings[name][MAX] += end_##name;               \
        timings[name][MEAN] += end_##name;              \
    } while (0)

#define PRINT_TIMING(name) do {                                         \
        MPI_Allreduce(MPI_IN_PLACE, &timings[name][MAX], 1, MPI_DOUBLE, \
                      MPI_MAX, COMM);                                   \
        MPI_Allreduce(MPI_IN_PLACE, &timings[name][MIN], 1, MPI_DOUBLE, \
                      MPI_MIN, COMM);                                   \
        MPI_Allreduce(MPI_IN_PLACE, &timings[name][MEAN], 1, MPI_DOUBLE,\
                      MPI_SUM, COMM);                                   \
        if ( !get_rank() ) {                                            \
            printf("%s [min, max, mean]: %f, %f %f\n",                  \
                   timing_str[name],                                    \
                   timings[name][MIN],                                  \
                   timings[name][MAX],                                  \
                   timings[name][MEAN]/get_size());                     \
        }                                                               \
    } while (0)

static int rank_ = -1;
static int size_ = -1;

static int file_sizes[] = {230000 * 1024 / sizeof(T),
                           240000 * 1024 / sizeof(T),
                           250000 * 1024 / sizeof(T),
                           260000 * 1024 / sizeof(T),
                           700000 * 1024 / sizeof(T),
                           780000 * 1024 / sizeof(T)};

enum files { K = 0, NUT, OMEGA, P, PHI, U, INVALID };

static char *file_str[] = {
    ITEM(K),
    ITEM(NUT),
    ITEM(OMEGA),
    ITEM(P),
    ITEM(PHI),
    ITEM(U)
};

#undef ITEM
#define N_FILES INVALID

static inline int get_size()
{
    if ( size_ == -1 ) {
        MPI_Comm_size(COMM, &size_);
    }
    return size_;
}
static inline int get_rank()
{
    if ( rank_ == -1 ) {
        MPI_Comm_rank(COMM, &rank_);
    }
    return rank_;
}

static void ensure_directory(const char *base, int timestep)
{
    char *dir = NULL;
    int ret;
    ret = asprintf(&dir, "%s/%d", base, timestep);
    if ( ret < 0 ) {
        fprintf(stderr, "[%d] Failed to allocate space for directory name\n",
                get_rank());
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    ret = mkdir(base, S_IRWXU);
    if ( ret == -1 && errno != EEXIST ) {
        perror("mkdir base failed\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    ret = mkdir(dir, S_IRWXU);
    if ( ret == -1 && errno != EEXIST ) {
        perror("mkdir subdir failed\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    free(dir);
}

static char *get_file_name(const char *directory, const char *basename,
                           int which, int timestep)
{
    char *name = NULL;
    int ret;
    ret = asprintf(&name, "%s/%d/%s-%s",
                   directory, timestep, basename, file_str[which]);
    if ( ret < 0 ) {
        fprintf(stderr, "[%d] Failed to allocate space for filename\n",
                get_rank());
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    return name;
}

static void alloc_data(T **data, int nitems)
{
    *data = malloc(nitems * sizeof(**data));
}

static void dealloc_data(void *data)
{
    free(data);
}

static void fake_input(int *nitems, int i)
{
    *nitems = file_sizes[i] / get_size();
}

static void init_data(T *data, int nitems, int val)
{
    int i;
    for ( i = 0; i < nitems; i++ ) {
        data[i] = (T)val;
    }
}

#endif
