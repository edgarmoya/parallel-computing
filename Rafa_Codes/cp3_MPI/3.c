/*  
   Rafael_Fernandez_Fleites
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int rank, size;
    int rows, cols;
    int* matrix;
    int* localRowSums;
    int* rowSums;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    //Lectura de filas y columnas, inicialización de la matriz
    if (rank == 0) {
        printf("Ingrese el número de filas: ");
        scanf("%d", &rows);

        printf("Ingrese el número de columnas: ");
        scanf("%d", &cols);

        if (rows % size != 0) {
            printf("El número de filas debe ser múltiplo de la cantidad de procesos.\n");
            MPI_Finalize();
            return 0;
        }

        srand(time(NULL));
        matrix = (int*)malloc(sizeof(int) * rows * cols);
        printf("La matriz queda de la siguiente forma:\n");
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                matrix[i * cols + j] = rand() % 10;
                printf("%d ",matrix[i * cols + j]);
            }
            printf("\n");
        }   
    }

    // Enviar el número de filas y columnas a todos los procesos
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular el número de filas por proceso
    int rowsPerProcess = rows / size;
    int elementsPerProcess = rowsPerProcess*cols;

    // Asignar memoria para almacenar las sumas locales de cada proceso
    int* localRowsDist = (int*)malloc(sizeof(int) * elementsPerProcess);
    localRowSums = (int*)malloc(sizeof(int) * rowsPerProcess);

    // Distribuir las filas de la matriz entre los procesos
    MPI_Scatter(matrix, elementsPerProcess, MPI_INT, localRowsDist, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular la suma de las filas locales
    int localSum;
    for (int i = 0; i < rowsPerProcess; i++) {
        localSum = 0;
        for (int j = 0; j < cols; j++) {
            localSum += localRowsDist[i * cols + j];
        }
        localRowSums[i] = localSum;
    }

    // Reunir el valor de la suma de cada fila en el proceso 0
    if (rank == 0) {
        rowSums = (int*)malloc(sizeof(int) * rows);
    }
    MPI_Gather(localRowSums, rowsPerProcess, MPI_INT, rowSums, rowsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);

    // Imprimir el vector resultante en el proceso 0
    if (rank == 0) {
        printf("Suma de elementos de cada fila:\n");
        for (int i = 0; i < rows; i++) {
            printf("Fila %d suma:%d\n",i, rowSums[i]);
        }
        printf("\n");
    }
    // Liberar memoria
    if (rank == 0) {
        free(matrix);
        free(rowSums);
    }
    free(localRowSums);
    free(localRowsDist);

    MPI_Finalize();
    return 0;
}