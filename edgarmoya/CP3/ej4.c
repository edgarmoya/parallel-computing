/* Escriba un programa paralelo que permita calcular la suma de cada una de las columnas 
de una matriz. El proceso 0 lee la cantidad de filas y columnas (El número de columnas 
múltiplo de la cantidad de procesos) e inicializa con valores aleatorios la matriz. Las 
sumas se realizan en paralelo y luego el proceso 0 imprime el vector resultante */

/* Edgar Moya Cáceres */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void print_matrix(int *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int rows, cols, cols_x_process;
    int *matrix = NULL;
    int *local_matrix = NULL;
    int *transposed_matrix = NULL;
    int *column_sum = NULL;
    int *global_column_sum = NULL;

    if (rank == 0) {
        printf("Ingrese el número de filas: \n");
        scanf("%d", &rows);
        printf("Ingrese el número de columnas (múltiplo del número de procesos):\n");
        scanf("%d", &cols);

        // Comprobar que el número de columnas es múltiplo de la cantidad de procesos
        if (cols % size != 0) {
            printf("El número de columnas debe ser múltiplo del número de procesos.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        // Asignación de memoria para la matriz
        matrix = (int *)malloc(rows * cols * sizeof(int));

        // Inicializar la matriz con valores aleatorios
        srand(time(NULL));
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                matrix[i * cols + j] = rand() % 10;
            }
        }

        printf("Matriz inicial:\n");
        print_matrix(matrix, rows, cols);

        // Transponer matriz
        transposed_matrix = (int *)malloc(cols * rows * sizeof(int));
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                transposed_matrix[j * rows + i] = matrix[i * cols + j];
            }
        }
    }

    // Difundir el tamaño de la matriz a todos los procesos
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular la cantidad de columnas por proceso
    cols_x_process = cols / size;

    // Distribuir las columnas de la matriz a todos los procesos
    local_matrix = (int *)malloc(cols_x_process * rows * sizeof(int));
    MPI_Scatter(transposed_matrix, cols_x_process * rows, MPI_INT, local_matrix, cols_x_process * rows, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular la suma de cada columna localmente
    column_sum = (int *)malloc(cols_x_process * sizeof(int));
    for (int i = 0; i < cols_x_process; i++) {
        column_sum[i] = 0;
        for (int j = 0; j < rows; j++) {
            column_sum[i] += local_matrix[i * rows + j];
        }
    }

    // Recolectar las sumas de las columnas de todos los procesos
    if (rank == 0) {
        global_column_sum = (int *)malloc(cols * sizeof(int));
    }
    MPI_Gather(column_sum, cols_x_process, MPI_INT, global_column_sum, cols_x_process, MPI_INT, 0, MPI_COMM_WORLD);

    // Proceso 0 imprime el vector resultante con las sumas de cada columna
    if (rank == 0) {
        printf("\nSuma de los elementos de cada columna:\n");
        for (int i = 0; i < cols; i++) {
            printf("Columna %d -> %d\n", i+1, global_column_sum[i]);
        }
    }

    // Liberar la memoria
    free(column_sum);
    free(local_matrix);

    if (rank == 0) {
        free(transposed_matrix);
        free(matrix);
        free(global_column_sum);
    }

    MPI_Finalize();
    return 0;
}
