#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int provided;
    int rank;

    // Inicialización de MPI con soporte de subprocesos MPI_THREAD_FUNNELED
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    // Verificación del nivel de soporte de subprocesos proporcionado por MPI
    if (provided != MPI_THREAD_FUNNELED) {
        fprintf(stderr, "Warning MPI did not provide MPI_THREAD_FUNNELED\n");
        exit(-1);
    }

    // Obtención del rango del proceso MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Inicio de la región paralela de OpenMP
    #pragma omp parallel default(none), \
    shared(rank), \
    shared(ompi_mpi_comm_world),\
    shared(ompi_mpi_int), \
    shared(ompi_mpi_char)
    // Las variables ompi_mpi_xx. Requeridas por versiones
    // superiores de 6.1 de gcc
    {
        printf("Hello from thread %d at rank %d parallel region\n",omp_get_thread_num(), rank);
        
        // Sección crítica controlada por el hilo maestro (master thread) de OpenMP
        #pragma omp master
        {
            char helloWorld[13]; // Para tener en cuenta en espacio y el final del string: '/0'

            if (rank == 0) {
                strcpy(helloWorld, "Hello World");

                // Envío del mensaje al proceso con rango 1 mediante MPI_Send
                MPI_Send(helloWorld, 13, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                
                printf("Rank %d send: %s\n", rank, helloWorld);
            }
            else {

                // Recepción del mensaje desde el proceso con rango 0 mediante MPI_Recv
                MPI_Recv(helloWorld, 13, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                
                printf("Rank %d received: %s\n", rank, helloWorld);
            }
        }
    }

    //MPI_Finalize();
}