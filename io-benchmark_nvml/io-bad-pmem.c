#define _GNU_SOURCE
#include "common.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpmem.h>

// PMEM_IS_PMEM_FORCE=1 ./io-bad-pmem XXXX

/* copying 4k at a time to pmem for this example */
#define BUF_LEN 4096
/*
 * do_copy_to_pmem -- copy to pmem, postponing drain step until the end
 */
static void
do_copy_to_pmem(char *pmemaddr, T *data, int nitems)
{
  pmem_memcpy_nodrain(pmemaddr, data, nitems*sizeof(int));

  /* perform final flush step */
  pmem_drain();
}

int main(int argc, char **argv)
{
    T **data;
    int *nitems;
    char *directory = NULL;
    char *outname;
    int timestep;
    int i;
    char **pmemaddr;
    int is_pmem;
    size_t mapped_len;
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
                } else {
                    if ( !get_rank() ) {
                        fprintf(stderr, "Usage: %s DIRECTORY OUTFILE [INFILE]\n", basename(argv[0]));

                    }
                    MPI_Finalize();
                    return -1;
                }

                pmemaddr = malloc(N_FILES * sizeof(char*));
                data = malloc(N_FILES * sizeof(*data));
                nitems = malloc(N_FILES * sizeof(*nitems));
                for ( timestep = 0; timestep < MAX_TIMESTEPS; timestep++ ) {
                    WITH_TIMING(ENSURE_DIRECTORY,
                                ensure_directory(directory, timestep));
                    WITH_TIMING(FAKE_INPUT,
                                for ( i = 0; i < N_FILES; i++ ) {
                                    fake_input(&(nitems[i]), i);
                                    alloc_data(&(data[i]), nitems[i]);
                                    init_data(data[i], nitems[i], get_rank());
                                });

                    WITH_TIMING(OPEN_OUTPUT,
                                for ( i = 0; i < N_FILES; i++ ) {
                                    char *fname = get_file_name(directory, outname, i, timestep);
                                    if ((pmemaddr[i] = pmem_map_file(fname, nitems[i]*sizeof(int),
                                         PMEM_FILE_CREATE|PMEM_FILE_EXCL,
                                         0666, &mapped_len, &is_pmem)) == NULL) {
                                           perror("pmem_map_file");
                                           fprintf(stderr, "[%d] Failed to pmem_map_file for filename:%s.\n", get_rank(), fname);
                                           exit(1);
                                         }
                                    free(fname);
                                });

                    WITH_TIMING(WRITE_OUTPUT,
                                for ( i = 0; i < N_FILES; i++ ) {
                                    do_copy_to_pmem(pmemaddr[i], data[i], nitems[i]);
                                });

                    WITH_TIMING(CLOSE_OUTPUT,
                                for ( i = 0; i < N_FILES; i++ ) {
                                    pmem_unmap(pmemaddr[i], mapped_len);
                                });
                    for ( i = 0; i < N_FILES; i++ ) {
                        dealloc_data(data[i]);
                    }
                }
                free(data);   free(nitems);    free(directory);    free(pmemaddr);
                MPI_Barrier(COMM);
        );
    for ( i = 0; i < INVALID_TIMING; i++ ) {
        PRINT_TIMING(i);
    }
    MPI_Finalize();

    return 0;
}
