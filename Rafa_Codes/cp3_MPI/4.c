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
    int* localColsSums;
    int* colsSums;
    // int* transposedMatrix;
    // int* sendcounts;
    // int* displs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    //Lectura de filas y columnas, inicialización de la matriz
    if (rank == 0) {
        printf("Ingrese el número de filas: ");
        scanf("%d", &rows);

        printf("Ingrese el número de columnas: ");
        scanf("%d", &cols);

        if (cols % size != 0) {
            printf("El número de columnas debe ser múltiplo de la cantidad de procesos.\n");
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
        // transposedMatrix = (int*)malloc(sizeof(int) * cols * rows);
        // for (int i = 0; i < cols; i++) {
        //     for (int j = 0; j < rows; j++) {
        //         transposedMatrix[i * rows + j] = matrix[j * cols + i];
        //     }
        // }
    }

    // Enviar el número de filas y columnas a todos los procesos
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular el número de filas por proceso
    int colsPerProcess = cols / size;
    int elementsPerProcess = colsPerProcess * rows;

    // Creando la submatriz de columnas
    MPI_Datatype col, coltype;
    MPI_Type_vector(rows, 1, cols, MPI_INT, &col);
    MPI_Type_commit(&col);
    // https://stackoverflow.com/questions/10788180/sending-columns-of-a-matrix-using-mpi-scatter
    MPI_Type_create_resized(col, 0, 1*sizeof(int), &coltype);
    MPI_Type_commit(&coltype);

    // Asignar memoria para almacenar las sumas locales de cada proceso
    int* localColsDist = (int*)malloc(sizeof(int) * elementsPerProcess);
    localColsSums = (int*)malloc(sizeof(int) * colsPerProcess);

    // Distribuir las filas de la matriz entre los procesos
    MPI_Scatter(matrix, colsPerProcess, coltype, localColsDist, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);

    // for(int i = 0; i < elementsPerProcess; i++){
    //     printf("%d ",localColsDist[i]);
    // }
    // printf("%d\n",rank);

    // Calcular la suma de las filas locales
    int localSum;
    for (int i = 0; i < colsPerProcess; i++) {
        localSum = 0;
        for (int j = 0; j < rows; j++) {
            localSum += localColsDist[i * rows + j];
        }
        localColsSums[i] = localSum;
    }

    // Reunir el valor de la suma de cada fila en el proceso 0
    if (rank == 0) {
        colsSums = (int*)malloc(sizeof(int) * rows);
    }
    MPI_Gather(localColsSums, colsPerProcess, MPI_INT, colsSums, colsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);

    // Imprimir el vector resultante en el proceso 0
    if (rank == 0) {
        printf("Suma de elementos de cada columna:\n");
        for (int i = 0; i < cols; i++) {
            printf("Columna %d suma:%d\n",i, colsSums[i]);
        }
        printf("\n");
    }
    // Liberar memoria
    if (rank == 0) {
        free(matrix);
        free(colsSums);
    }
    free(localColsSums);
    free(localColsDist);

    MPI_Finalize();
    return 0;
}