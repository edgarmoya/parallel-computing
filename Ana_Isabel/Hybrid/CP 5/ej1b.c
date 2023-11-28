/******************************************************************************
Ana Isabel Portal Díaz

Ejercicio 1

Escriba un programa paralelo híbrido(MPI + OpenMP) que permita calcular la suma de
cada una de las filas de una matriz. La cantidad de filas y columnas( El número de filas es
múltiplo de la cantidad de procesos) se pasan como argumentos al programa. El proceso 0
inicializa con valores aleatorios la matriz. Las sumas se realizan en paralelo y luego el proceso
0 imprime el vector resultante.

A) Considere el caso en que la cantidad de filas pueda tomar valores no múltiplos del
número de procesos.
B) Calcule el speedup (S = Ts/Tp) para distintos valores de dimensión de la matriz
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>

void innit_matrix(int *array, const int rows, const int cols, const int bound) {
    srand(time(NULL));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            array[i * cols + j] = rand() % 10;
        }
    }
}

void print_vector(int *vector, int cols){
    for (int j = 0; j < cols; j++) {
        printf("%d ", vector[j]);
    }
    printf("\n");    
}

void print_matrix(int *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int rank, size, n_threads;
    int rows, cols, rows_per_process, remainder_rows;
    MPI_Datatype dtype_rows;
    int *matrix = NULL;
    int *local_matrix = NULL;
    int *send_counts = NULL;
    int *displacements = NULL;
    int *parallel_row_sum = NULL;
    int *sequential_row_sum = NULL;
    int *global_row_sum = NULL;

    double p_start_time, p_end_time, p_time;
    clock_t s_start_time, s_end_time ; 
    double s_time;

    double SpeedUp,eficacia;  

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        
        //Comprobar requerimientos         
        if (argc != 4) {
            printf("Uso: %s <cantidad de hilos> <filas> <columnas> \n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        //Obtener cant de filas y columnas
        if ((rows = atoi(argv[2])) == 0 || (cols = atoi(argv[3])) == 0) {
            printf("Las filas y las columnas deben ser números enteros.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        // Asignar memoria para la matriz
        matrix = (int *)malloc( rows * cols * sizeof(int));

        // Inicializar la matriz con valores aleatorios
        innit_matrix( matrix, rows, cols, 10);

        //Imprimir la matriz
        // printf("Matriz inicial:\n");        
        // print_matrix(matrix, rows, cols);
        
    }

    // Difundir el tamaño de la matriz a todos los procesos
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Type_vector(1, cols, cols, MPI_INT, &dtype_rows);
    MPI_Type_commit(&dtype_rows);

    // Calcular la cantidad de filas por proceso 
    rows_per_process = rows / size;
    // Calcular la cantidad de filas restantes
    remainder_rows = rows % size;

    // Calcular la cantidad de filas a asignadas a cada procesos 
    send_counts = (int *)malloc(size * sizeof(int));
    displacements = (int *)malloc(size * sizeof(int));
    
    int actual_displacement = 0;

    for (int i = 0; i < size; i++) {
        send_counts[i] = (i < remainder_rows) ? (rows_per_process + 1) : rows_per_process;
        displacements[i] = actual_displacement;
        actual_displacement += send_counts[i];
    }

    // Distribuir las filas de la matriz a todos los procesos
    local_matrix = (int *)malloc(send_counts[rank] * cols * sizeof(int));
    MPI_Scatterv(matrix, send_counts, displacements, dtype_rows, local_matrix, send_counts[rank], dtype_rows, 0, MPI_COMM_WORLD);

    // Tiempo en que se inicia
    p_start_time = MPI_Wtime();

    // Configurar cantidad de hilos
    omp_set_num_threads(atoi(argv[1]));
    
    /****************************************
     * Calcular la suma de cada fila localmente  
    ******************************************/ 
    int i = 0, j = 0;
    parallel_row_sum = (int *)malloc(send_counts[rank] * sizeof(int));
    #pragma omp parallel for private(i, j) shared(matrix, parallel_row_sum, local_matrix, cols)
        for (i = 0; i < send_counts[rank]; i++) {
            parallel_row_sum[i] = 0;
            for (j = 0; j < cols; j++) {
                parallel_row_sum[i] += local_matrix[i * cols + j];
            }
        }

    // Recolectar las sumas de las filas de todos los procesos
    if (rank == 0)
        global_row_sum = (int *)malloc(rows * sizeof(int));

    MPI_Gatherv(parallel_row_sum, send_counts[rank], MPI_INT, global_row_sum, send_counts, displacements, MPI_INT, 0, MPI_COMM_WORLD);

    // Detener y calcular el tiempo de ejecución
    p_end_time = MPI_Wtime();
    p_time = p_end_time - p_start_time;

    // Imprimir el tiempo de ejecución
    if (rank == 0)
        printf("Tiempo de ejecución (paralelo): %.6f seg\n", p_time);

    // Liberar memoria
    free(parallel_row_sum);
    free(local_matrix);
    free(send_counts);
    free(displacements);

    if (rank == 0)
        free(global_row_sum);
    

    /****************************************** 
     * 
     * SECUENCIAL 
     * 
    ********************************************/

    // Calcular la suma de cada fila secuencialmente
    if (rank == 0) {

        // Medir tiempo antes de calcular la suma de cada fila
        s_start_time = clock();

        sequential_row_sum = (int *)malloc(rows * sizeof(int));
        for (i = 0; i < rows; i++) {
            sequential_row_sum[i] = 0;
            for (j = 0; j < cols; j++) {
                sequential_row_sum[i] += matrix[i * cols + j];
            }
        }

        // Medir tiempo después de calcular la suma de cada fila
        s_end_time = clock();

        // Tiempo total en secuencial
        s_time = (double)(s_end_time-s_start_time) / CLOCKS_PER_SEC;

        // Imprimir el tiempo de ejecución secuencial
        printf("Tiempo de ejecución (secuencial): %.6f seg\n", s_time);

        // Liberar memoria
        free(sequential_row_sum);
        free(matrix);
    }

    if (rank==0){
        SpeedUp = s_time / p_time;
        printf("El speedup (St/Sp) es: %.6f\n", SpeedUp);

        eficacia = SpeedUp / size;
        printf("La eficacia es: %.6f\n", eficacia);
        
    }

    MPI_Finalize();
    return 0;
}