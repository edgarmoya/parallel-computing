/* Calcular tiempo */
/* Edgar Moya Cáceres */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int rank, size;
    int rows, cols, rows_x_process;
    int *matrix = NULL;
    int *local_matrix = NULL;
    int *row_sum = NULL;
    int *global_row_sum = NULL;
    double start_time, end_time;

    MPI_Init(&argc, &argv);

    // Iniciar medición del tiempo
    start_time = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        rows = 20000;
        cols = 20000;

        // Comprobar que el número de filas es múltiplo de la cantidad de procesos
        if (rows % size != 0) {
            printf("El número de filas debe ser múltiplo de la cantidad de procesos.\n");
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
    }

    // Difundir el tamaño de la matriz a todos los procesos
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular la cantidad de filas por proceso
    rows_x_process = rows / size;

    // Distribuir las filas de la matriz a todos los procesos
    local_matrix = (int *)malloc(rows_x_process * cols * sizeof(int));
    MPI_Scatter(matrix, rows_x_process * cols, MPI_INT, local_matrix, rows_x_process * cols, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular la suma de cada fila localmente
    row_sum = (int *)malloc(rows_x_process * sizeof(int));
    for (int i = 0; i < rows_x_process; i++) {
        row_sum[i] = 0;
        for (int j = 0; j < cols; j++) {
            row_sum[i] += local_matrix[i * cols + j];
        }
    }

    // Recolectar las sumas de las filas de todos los procesos
    if (rank == 0) {
        global_row_sum = (int *)malloc(rows * sizeof(int));
    }
    MPI_Gather(row_sum, rows_x_process, MPI_INT, global_row_sum, rows_x_process, MPI_INT, 0, MPI_COMM_WORLD);

    // Liberar la memoria
    free(row_sum);
    free(local_matrix);

    if (rank == 0) {
        free(matrix);
        free(global_row_sum);
    }

    end_time = MPI_Wtime();

    // Calcular el tiempo de ejecución en paralelo
    double execution_time = end_time - start_time;

    if (rank == 0) {
        // Imprimir el tiempo de ejecución en paralelo
        printf("Tiempo de ejecución: %.5f segundos\n", execution_time);
    }

    MPI_Finalize();
    return 0;
}
