/*
    Rafael_Fernandez_Fleites
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char *argv[])
{ 
    long i;
    double x, sum = 0.0;
    int rank, size;
    long it_per_proces, num_steps;
    double step, pi;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0){

        if (argc != 2) {
            printf("Uso: %s <cantidad de pasos>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        
        if ((num_steps = atol(argv[1])) == 0) {
            printf("El número de pasos debe ser un número entero.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        if (num_steps % size != 0)
        {
            printf("El número de pasos debe ser múltiplo de la cantidad de procesos.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
            return 0;
        }

        it_per_proces = num_steps / (long) size;
        step = 1.0/(double) num_steps;
    }

    MPI_Bcast(&it_per_proces, 1, MPI_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&step, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    long init = (long)rank*it_per_proces;
    long end = (long)(rank+1)*it_per_proces;

    double local_sum = 0.0;

    //printf("Procces %d it_per_proces:%ld init: %ld end %ld\n",rank, it_per_proces, init, end);

    omp_set_num_threads(8);

    #pragma omp parallel for private(x) reduction(+:local_sum)
    for (i = init; i < end ; i++){
        x = (i+0.5)*step;
        local_sum = local_sum + 4.0/(1.0 + x*x);
    }

    MPI_Reduce(&local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank == 0){
        pi = step * sum;
        printf("Pi = %.24f\n",pi);
    }
    
    MPI_Finalize();

}
