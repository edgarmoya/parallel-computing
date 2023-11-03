/* Calculando el tiempo de ejecución con 1 y 4 procesos para un vector 
de tamaño 800000 utilizando MPI_Wtime */

/* Edgar Moya Cáceres */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int rank, size;
    int elements_x_proc;
    int *V = NULL;
    int *local_V = NULL;
    int N, local_sum = 0, global_sum = 0;
    double start_time, end_time;

    MPI_Init(&argc, &argv);

    start_time = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Tamaño del vector
        N = 800000;

        // Comprobar que el tamaño es múltiplo de la cantidad de procesos
        if (N % size != 0) {
            printf("El tamaño debe ser múltiplo de la cantidad de procesos.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        // Generar un vector con números aleatorios
        V = (int *)malloc(N * sizeof(int));
        srand(time(NULL));
        for (int i = 0; i < N; i++){
            V[i] = rand() % (100-50);
        }

        printf("Vector inicial: \n");
        for (int i = 0; i < N; i++){
            printf("%d ", V[i]);
        }
        printf("\n");
    }

    // Difundir el tamaño del vector a todos los procesos
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Cantidad de elementos por cada proceso
    elements_x_proc = N / size;

    // Distribuir los elementos por cada proceso
    local_V = (int *)malloc(elements_x_proc * sizeof(int));
    MPI_Scatter(V, elements_x_proc, MPI_INT, local_V, elements_x_proc, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular la suma de cada porción del vector
    for (int i = 0; i < elements_x_proc; i++){
        local_sum += local_V[i];
    }
        
    // Calcular total y enviar el resultado a todos los procesos
    MPI_Allreduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // Multplicar cada elemento por el total
    for (int i = 0; i < elements_x_proc; i++){
        local_V[i] = local_V[i] * global_sum;
    }

    // Recolectar todos las porciones en V ya multiplicadas por la suma total
    MPI_Gather(local_V, elements_x_proc, MPI_INT, V, elements_x_proc, MPI_INT, 0, MPI_COMM_WORLD);

    // Imprimir resultados
    if (rank == 0) {
        printf("\nSum = %d\n", global_sum);
        printf("V[0] = %d, V[N/2] = %d, V[N-1] = %d\n", V[0], V[N/2], V[N-1]);
    }

    // Liberar memoria
    free(local_V);
    free(V);

    // Calcular el tiempo de ejecución
    end_time = MPI_Wtime();
    double execution_time = end_time - start_time;

    // Imprimir el tiempo de ejecución
    if (rank == 0){
        printf("Tiempo de ejecución: %.6f seg\n", execution_time);
    }

    MPI_Finalize();
    return 0;
}
