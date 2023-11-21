/******************************************************************************
Ana Isabel Portal Díaz

Ejercicio 1

Escriba un programa paralelo híbrido(MPI + OpenMP) que permita calcular la suma de
cada una de las filas de una matriz. La cantidad de filas y columnas( El número de filas es
múltiplo de la cantidad de procesos) se pasan como argumentos al programa. El proceso 0
inicializa con valores aleatorios la matriz. Las sumas se realizan en paralelo y luego el proceso
0 imprime el vector resultante.

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



int main(int argc, char *argv[]){

    int rank, size;
    int rows, cols, rows_per_process;
    int *matrix = NULL;
    int *local_matrix = NULL;
    int *parallel_row_sum = NULL;
    int *row_sum = NULL;
    int *global_row_sum = NULL;

    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Cantidad de procesos: %d\n",size);
    printf("Soy el proceso: %d\n",rank);

    if(rank==0){

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

        //Comprobar que el numero de filas es multiplo de la cantidad de procesos
        if (rows % size != 0) {
            printf("El número de filas debe ser múltiplo de la cantidad de procesos.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
            return 0;
        }

        matrix = (int *)malloc(rows * cols * sizeof(int));

        //Inicializar la matriz
        innit_matrix( matrix, rows, cols, 10);

        //Imprimir la matriz
        printf("Matriz inicial:\n");        
        print_matrix(matrix, rows, cols);      

    }

    // Difundir el tamaño de la matriz a todos los procesos
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular la cantidad de filas por proceso 
    rows_per_process = rows / size;

    // Distribuir las filas de la matriz a todos los procesos
    local_matrix = (int *)malloc(rows_per_process * cols * sizeof(int));
    MPI_Scatter(matrix, rows_per_process * cols, MPI_INT, local_matrix, rows_per_process * cols, MPI_INT, 0, MPI_COMM_WORLD);

    // Configurar cantidad de hilos
    omp_set_num_threads(atoi(argv[1]));

    int i = 0, j = 0;
    parallel_row_sum = (int *)malloc(rows_per_process * sizeof(int));

    /****************************************
     * Calcular la suma de cada fila localmente  
    ******************************************/ 
    #pragma omp parallel for private(i, j) shared(matrix, parallel_row_sum, local_matrix, cols)
        for (i = 0; i < rows_per_process; i++) {
            parallel_row_sum[i] = 0;
            for (j = 0; j < cols; j++) {
                parallel_row_sum[i] += local_matrix[i * cols + j];
            }
        }

    // Recolectar las sumas de las filas de todos los procesos
    if (rank == 0) 
        global_row_sum = (int *)malloc(rows * sizeof(int));
    
    MPI_Gather(parallel_row_sum, rows_per_process, MPI_INT, global_row_sum, rows_per_process, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0){
        printf("El vector de la suma de las filas:\n");
        print_vector(global_row_sum, rows);            
    }

    // Liberar la memoria
    free(parallel_row_sum);
    free(local_matrix);

    if (rank == 0) {
        free(matrix);
        free(global_row_sum);
    }
    
    MPI_Finalize();     
    return 0;
}

