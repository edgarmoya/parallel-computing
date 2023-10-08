#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROWS 3
#define COLS 4

void printMatrix(int matrix[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int matrix[ROWS][COLS];

    if (rank == 0) {
        // Inicializar la matriz
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                matrix[i][j] = i * COLS + j;
            }
        }

        printf("Matriz original:\n");
        printMatrix(matrix);
    }

    // Crear el tipo de datos para la matriz traspuesta
    MPI_Datatype transposeType;
    MPI_Type_vector(COLS, 1, ROWS, MPI_INT, &transposeType);
    MPI_Type_commit(&transposeType);

    // Distribuir las columnas traspuestas de la matriz entre los procesos
    MPI_Scatter(matrix, 1, transposeType, matrix, COLS, MPI_INT, 0, MPI_COMM_WORLD);

    // Imprimir la parte de la matriz recibida por cada proceso
    printf("Proceso %d - Matriz recibida:\n", rank);
    printMatrix(matrix);

    // Transponer la matriz en cada proceso
    int transposedMatrix[COLS][ROWS];
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            transposedMatrix[i][j] = matrix[j][i];
        }
    }

    // Recopilar las filas traspuestas en el proceso 0
    MPI_Gather(transposedMatrix, COLS, MPI_INT, matrix, 1, transposeType, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Matriz traspuesta:\n");
        printMatrix(matrix);
    }

    // Liberar recursos
    MPI_Type_free(&transposeType);

    MPI_Finalize();
    return 0;
}