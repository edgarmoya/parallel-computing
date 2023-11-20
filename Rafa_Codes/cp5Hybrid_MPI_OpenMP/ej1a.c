/*
    Rafael_Fernandez_Fleites
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
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
    int *counts_send, *displace_send, *counts_recv, *displace_recv;
    int rows, cols, rowsPerProcess, remaining_rows;

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

        if (argc != 3)
        {
            printf("Uso: %s <cantidad de filas> <canitdad de columnas>\n", argv[0]);
            return 0;
        }

        if ((rows = atoi(argv[1])) == 0 || (cols = atoi(argv[2])) == 0)
        {
            printf("Las filas y las columnas deben ser números enteros.\n");
            return 0;
        }

        matrix = (int *)malloc(rows * cols * sizeof(int));
        randInit(matrix, cols * rows, 10);

        // printf("La matriz queda con la forma:\n");

        // for (int i = 0; i < rows; i++)
        // {
        //     for (int j = 0; j < cols; j++)
        //     {
        //         printf("%d ", matrix[i * cols + j]);
        //     }
        //     printf("\n");
        // }

        counts_send = (int *)malloc(sizeof(int) * size);
        displace_send = (int *)malloc(sizeof(int) * size);
        counts_recv = (int *)malloc(sizeof(int) * size);
        displace_recv = (int *)malloc(sizeof(int) * size);
        remaining_rows = rows % size;
        rowsPerProcess = rows / size;
        displace_recv[0] = 0;
        displace_send[0] = 0;
        for (int i = 0, j = remaining_rows; i < size; i++, j--)
        {
            counts_send[i] = rowsPerProcess * cols;
            counts_recv[i] = rowsPerProcess;
            if (j > 0)
            {
                counts_send[i] += cols;
                counts_recv[i]++;
            }
            if (i > 0)
            {
                displace_send[i] = displace_send[i - 1] + counts_send[i - 1];
                displace_recv[i] = displace_recv[i - 1] + counts_recv[i - 1];
            }
        }
    }
        
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    
    // Calcular tiempo de ejecución
    double itime, ftime, exec_time;

    omp_set_num_threads(8);

    rowsPerProcess = rows / size;
    remaining_rows = rows % size;
    
    if (rank < remaining_rows)
    {
        rowsPerProcess++;
    }
    int elementsPerProcess = rowsPerProcess * cols;

    // Asignar memoria para almacenar las sumas locales de cada proceso
    int *localRowsDist = (int *)malloc(sizeof(int) * elementsPerProcess);
    localSums = (int *)malloc(rowsPerProcess * sizeof(int));


    #pragma omp parallel shared(localRowsDist, localSums)
    {
        
        //printf("procces:%d thread:%d\n", rank, omp_get_thread_num());

        // Distribuir las filas de la matriz entre los procesos
        #pragma omp barrier
        #pragma omp master
        { 
        MPI_Scatterv(matrix, counts_send, displace_send, MPI_INT, localRowsDist, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);
        }
        #pragma omp barrier

        // Cada proceso realiza la suma de sus filas en paralelo
        
        itime = MPI_Wtime();

        #pragma omp for private(j)
            for (i = 0; i < rowsPerProcess; i++)
            {
                int sum = 0;
                for (j = 0; j < cols; j++)
                {
                    sum += localRowsDist[i * cols + j];
                }
                localSums[i] = sum;
            }
        
        ftime = MPI_Wtime();
    }
        // Se reúnen las sumas parciales en el proceso 0
        if (rank == 0)
        {
            rowSums = (int *)malloc(rows * sizeof(int));
        }

    #pragma omp parallel shared(localRowsDist, localSums)
    {
        #pragma omp barrier
        #pragma omp master
        {
        MPI_Gatherv(localSums, rowsPerProcess, MPI_INT, rowSums, counts_recv, displace_recv, MPI_INT, 0, MPI_COMM_WORLD);
        }
        #pragma omp barrier
    }

    // El proceso 0 imprime el vector resultante
    if (rank == 0)
    {
        // printf("Suma por filas hibrido:\n");
        // for (i = 0; i < rows; i++)
        // {
        //     printf("%d ", rowSums[i]);
        // }
        // printf("\n");

        // Resultados del tiempo de ejecución en paralelo
         
        exec_time = ftime - itime; 
        printf("Tiempo de ejecución en paralelo: %.4f\n", exec_time);

        // Calcula en serie para comparar
        double time_spent = 0.0;
        clock_t begin = clock();

        int *serial_rows_sum = (int *)malloc(sizeof(int) * rows);
        int sum;

        for(i = 0; i < rows; i++){
            sum = 0;
            for(j = 0; j < cols; j++){
                sum += matrix[i * cols + j];
            }
            serial_rows_sum[i] = sum;
        }
    
        // Resultados del tiempo de ejecución en paralelo
        clock_t end = clock();
        time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
        
        // printf("Suma por filas serie:\n");
        // for (i = 0; i < rows; i++)
        // {
        //     printf("%d ", serial_rows_sum[i]);
        // }
        // printf("\n");
        printf("Tiempo de ejecución en serie: %.4f\n", time_spent);
        free(serial_rows_sum);
    }
    

    if(rank == 0){
        free(rowSums);
        free(matrix);
        free(counts_recv);
        free(counts_send);
        free(displace_recv);
        free(displace_send);
    }
    free(localSums);
    free(localRowsDist);

    MPI_Finalize();

    return 0;
}