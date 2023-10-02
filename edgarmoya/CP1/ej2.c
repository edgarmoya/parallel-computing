/* Empleando la biblioteca MPI escriba un programa que permita el envío de un arreglo 
definido de forma dinámica entre cada uno de los procesos. La comunicación se 
efectuará en forma de anillo similar al ejemplo de la conferencia. El tamaño del vector 
se lee desde la terminal por el proceso 0, quien lo inicializa de forma aleatoria. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int *array = NULL;
    int array_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        scanf("%d", &array_size);
        srand(time(NULL));

        array = (int *)malloc(array_size * sizeof(int));
        for (int i = 0; i < array_size; i++) {
            array[i] = rand() % 100;
        }
    }

    // Se transmite el tamaño del arreglo a todos los procesos
    MPI_Bcast(&array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        array = (int *)malloc(array_size * sizeof(int));
        MPI_Recv(array, array_size, MPI_INT, rank-1, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Send(array, array_size, MPI_INT, (rank+1)%size, 20, MPI_COMM_WORLD);

    if (rank == 0) {
        MPI_Recv(array, array_size, MPI_INT, size-1, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Imprimir el arreglo recibido en cada proceso
    printf("Proceso %d, arreglo recibido: ", rank);
    for (int i = 0; i < array_size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    free(array);
    MPI_Finalize();
    return 0;
}
