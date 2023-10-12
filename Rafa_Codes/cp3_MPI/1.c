/*
        Rafael_Fernandez_Fleites
*/
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int comm_size;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    if (comm_size != 2)
    {
        printf("This application must be run with 2 MPI processes.\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Request request;
    MPI_Status status;

    if (my_rank == 0)
    {
        int value_sent = 12345;
        printf("[MPI process %d] I sent value %d.\n", my_rank, value_sent);
        MPI_Isend(&value_sent, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
        // Realiza otras operaciones mientras se envía el mensaje
        // ...
        MPI_Wait(&request, &status); // Espera a que se complete el envío
    }
    else
    {
        int value_received = 0;
        MPI_Irecv(&value_received, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
        // Realiza otras operaciones mientras se recibe el mensaje
        // ...
        MPI_Wait(&request, &status); // Espera a que se complete la recepción
        printf("[MPI process %d] I received value %d.\n", my_rank, value_received);
    }

    MPI_Finalize();
    return 0;
}