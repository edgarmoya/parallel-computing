/*
    Rafael_Fernandez_Fleites
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>

void randInit(int *array, const int n, const int bound)
{
    srand(time(NULL));
    for (int i = 0; i < n; i++)
        array[i] = rand() % bound;
}

int main(int argc, char *argv[])
{
    int rank, size;
    int i, j;
    int provide;
    int *matrix, *rowSums, *localSums;
    int rows, cols, rowsPerProcess;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provide);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (provide != MPI_THREAD_FUNNELED)
    {
        fprintf(stderr, "MPI not provide FUNNELED");
        exit(-1);
    }
    else
    {
        if (rank == 0)
            printf("MPI provide FUNNELED\n");
    }

    if (rank == 0)
    {

        if (argc != 3) {
            printf("Uso: %s <filas> <columnas>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        
        if ((rows = atoi(argv[1])) == 0 || (cols = atoi(argv[2])) == 0) {
            printf("Las filas y las columnas deben ser números enteros.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        if (rows % size != 0)
        {
            printf("El número de filas debe ser múltiplo de la cantidad de procesos.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
            return 0;
        }

        matrix = (int *)malloc(rows * cols * sizeof(int));
        randInit(matrix, cols * rows, 10);

        printf("La matriz queda con la forma:\n");

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                printf("%d ", matrix[i * cols + j]);
            }
            printf("\n");
        }
    }

    
             
    // Enviar el número de filas y columnas a todos los procesos
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
       

    rowsPerProcess = rows / size;

    // Se distribuye el número de filas a cada proceso
    int *localMatrix = (int *)malloc(rowsPerProcess * cols * sizeof(int));
    localSums = (int *)malloc(rowsPerProcess * sizeof(int));

    #pragma omp parallel shared(localMatrix, localSums)
    {    
        #pragma omp barrier
        #pragma omp master
        { 
            MPI_Scatter(matrix, rowsPerProcess * cols, MPI_INT, localMatrix, rowsPerProcess * cols, MPI_INT, 0, MPI_COMM_WORLD);
        }
        #pragma omp barrier

        // Cada proceso realiza la suma de sus filas en paralelo
        
        #pragma omp for private(j) 
            for (i = 0; i < rowsPerProcess; i++)
            {
                int sum = 0;
                for (j = 0; j < cols; j++)
                {
                    sum += localMatrix[i * cols + j];
                }
                localSums[i] = sum;
            }
    }
        // Se reúnen las sumas parciales en el proceso 0
        if (rank == 0)
        {
            rowSums = (int *)malloc(rows * sizeof(int));
        }
    #pragma omp parallel
    {
        #pragma omp barrier
        #pragma omp master
        {
            MPI_Gather(localSums, rowsPerProcess, MPI_INT, rowSums, rowsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);
        }
        #pragma omp barrier
    }

    // El proceso 0 imprime el vector resultante
    if (rank == 0)
    {
        printf("Row sums:\n");
        for (i = 0; i < rows; i++)
        {
            printf("%d ", rowSums[i]);
        }
        printf("\n");
            
    }
        
    

    if(rank == 0){
        free(rowSums);
        free(matrix);
    }

    free(localSums);
    free(localMatrix);

    MPI_Finalize();

    return 0;
}