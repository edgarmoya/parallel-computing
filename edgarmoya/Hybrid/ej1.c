/* Programa paralelo híbrido que permita calcular la suma de cada una de las filas
de una matriz e imprime el tiempo. La cantidad de filas y columnas (El número de 
filas es múltiplo de la cantidad de procesos) se pasan como argumentos al programa. 
Se implementó el programa secuencial para calcular el speedup */

/* Edgar Moya Cáceres */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int rows, cols, rows_x_process;
    int *matrix = NULL, *local_matrix = NULL;
    int *parallel_row_sum, *sequential_row_sum = NULL;
    int *global_row_sum = NULL;
    double parallel_start_time, parallel_end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Comprobar si se proporcionaron los argumentos requeridos
        if (argc != 4) {
            fprintf(stderr, "Uso: %s <filas> <columnas> <cantidad de hilos>\n", argv[0]);
            return 1;
        }

        // Obtener cantidad de filas y columnas
        if ((rows = atoi(argv[1])) == 0 || (cols = atoi(argv[2])) == 0) {
            printf("Las filas y columnas deben ser números enteros.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

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
    
    // Tiempo de inicio
    parallel_start_time = MPI_Wtime();
    
    // Configurar la cantidad de hilos a utilizar
    omp_set_num_threads(atoi(argv[3]));

    // Calcular la suma de cada fila localmente
    int i = 0, j = 0;
    parallel_row_sum = (int *)malloc(rows_x_process * sizeof(int));
    #pragma omp parallel for private(i, j) shared(matrix, parallel_row_sum, local_matrix, cols)
        for (i = 0; i < rows_x_process; i++) {
            parallel_row_sum[i] = 0;
            for (j = 0; j < cols; j++) {
                parallel_row_sum[i] += local_matrix[i * cols + j];
            }
        }

    // Recolectar las sumas de las filas de todos los procesos
    if (rank == 0) {
        global_row_sum = (int *)malloc(rows * sizeof(int));
    }
    MPI_Gather(parallel_row_sum, rows_x_process, MPI_INT, global_row_sum, rows_x_process, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular el tiempo de ejecución
    parallel_end_time = MPI_Wtime();

    // Imprimir el tiempo de ejecución
    if (rank == 0){
        printf("Tiempo de ejecución (paralelo): %.6f seg\n", parallel_end_time - parallel_start_time);
    }

    // Liberar la memoria
    free(parallel_row_sum);
    free(local_matrix);
    if (rank == 0) {
        free(global_row_sum);
    }

    /***** SECUENCIAL *****/

    // Calcular la suma de cada fila secuencialmente
    if (rank == 0) {
        // Medir el tiempo antes de calcular la suma de cada fila
        clock_t sequential_start_time = clock();

        sequential_row_sum = (int *)malloc(rows * sizeof(int));
        for (i = 0; i < rows; i++) {
            sequential_row_sum[i] = 0;
            for (j = 0; j < cols; j++) {
                sequential_row_sum[i] += matrix[i * cols + j];
            }
        }

        // Medir el tiempo después de calcular la suma de cada fila
        clock_t sequential_end_time = clock();

        // Imprimir el tiempo de ejecución secuencial
        printf("Tiempo de ejecución (secuencial): %.6f seg\n", (double)(sequential_end_time-sequential_start_time) / CLOCKS_PER_SEC);

        // Liberar la memoria
        free(sequential_row_sum);
        free(matrix);
    }

    MPI_Finalize();
    return 0;
}
