/*  
   Rafael_Fernandez_Fleites
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int rank, size;
    int N;
    int* V;
    int sumV;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    //Lectura de filas y columnas, inicialización de la matriz
    if (rank == 0) {
        printf("Ingrese la longitud del vector: ");
        scanf("%d", &N);

        if (N % size != 0) {
            printf("La longitud del vector debe ser múltiplo de la cantidad de procesos.\n");
            MPI_Finalize();
            return 0;
        }

        srand(time(NULL));
        V = (int *)malloc(sizeof(int) * N);
        for(int i = 0; i < N; i++) V[i] = rand() % 100 - 50; // inicializacion del vector
        printf("El vector resultante queda de la siguiente forma:\n");
        for(int i = 0; i < N; i++) printf("%d ",V[i]);
        printf("\n");
    }

    // Enviar el número de filas y columnas a todos los procesos
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular el número de filas por proceso
    int elementsPerProcess = N / size;

    // Asignar memoria para almacenar las sumas locales de cada proceso
    int* localVectDist = (int*)malloc(sizeof(int) * elementsPerProcess);

    // Distribuir las filas de la matriz entre los procesos
    MPI_Scatter(V, elementsPerProcess, MPI_INT, localVectDist, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular la suma del segmento del vector
    int localVsum = 0;
    for (int i = 0; i < elementsPerProcess; i++) {
        localVsum = localVsum + localVectDist[i];
    }

    // Reagrupar y repartir a todos la suma de las sumas de segmentos
    int total_sum = 0;
    MPI_Allreduce(&localVsum, &total_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    
    // printf("%d %d\n",rank,total_sum);

    // Multiplicar la suma total por el vector
    for (int i = 0; i < elementsPerProcess; i++) {
        localVectDist[i] = total_sum * localVectDist[i];
    }
    
    // Reunir los pedazos modificiados en 0 remplazando en V
    MPI_Gather(localVectDist, elementsPerProcess, MPI_INT, V, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);

    // Imprimir el vector resultante en el proceso 0
    if (rank == 0) {
        printf("\n sum = %d\n", total_sum);
        for (int i = 0; i < N; i++) {
            printf("V[%d] = %d\n",i, V[i]);
        }
    }
    // Liberar memoria
    if (rank == 0) {
        free(V);
    }
    free(localVectDist);

    MPI_Finalize();
    return 0;
}