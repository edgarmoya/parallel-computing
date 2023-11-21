#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>

int main(int argc, char *argv[]) {    
    int provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    if (provided != MPI_THREAD_FUNNELED) {
        fprintf(stderr,"Warning MPI did not provide MPI_THREAD_FUNNELED\n");
        exit(-1);
    }
    else 
        printf("MPI provide MPI_THREAD_FUNNELED\n");

    MPI_Finalize();
    return 0;
}
