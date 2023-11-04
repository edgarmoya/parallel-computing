/*
   Rafael_Fernandez_Fleites
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int rows, cols;
    int *matrix;
    int *rowSums;
    int *counts_send;
    int *displace_send;
    int *counts_recv;
    int *displace_recv;
    int rows_per_process;
    int remaining_rows;
    double start_time, end_time;

    MPI_Init(&argc, &argv);    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Lectura de filas y columnas, inicialización de la matriz
    if (rank == 0)
    {
        // printf("Ingrese el número de filas: ");
        // scanf("%d", &rows);

        // printf("Ingrese el número de columnas: ");
        // scanf("%d", &cols);

        if (argc != 3) {
            printf("Uso: %s <filas> <columnas>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        
        if ((rows = atoi(argv[1])) == 0 || (cols = atoi(argv[2])) == 0) {
            printf("Las filas y las columnas deben ser números enteros.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }


        srand(time(NULL));
        matrix = (int *)malloc(sizeof(int) * rows * cols);
        // printf("La matriz queda de la siguiente forma:\n");
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                matrix[i * cols + j] = rand() % 10;
                // printf("%d ", matrix[i * cols + j]);
            }
            // printf("\n");
        }
        // Calcular la cantidad de filas que se enviarán y reciviran de cada proceso,
        // los primeros procesos recibiran una fila adicional para complementar el resto
        counts_send = (int *)malloc(sizeof(int) * size);
        displace_send = (int *)malloc(sizeof(int) * size);
        counts_recv = (int *)malloc(sizeof(int) * size);
        displace_recv = (int *)malloc(sizeof(int) * size);
        remaining_rows = rows % size;
        rows_per_process = rows / size;
        displace_recv[0] = 0;
        displace_send[0] = 0;
        for (int i = 0, j = remaining_rows; i < size; i++, j--)
        {
            counts_send[i] = rows_per_process * cols;
            counts_recv[i] = rows_per_process;
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
    
    // Enviar el número de filas y columnas a todos los procesos
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    rows_per_process = rows / size;
    remaining_rows = rows % size;
    if (rank < remaining_rows)
    {
        rows_per_process++;
    }
    int elementsPerProcess = rows_per_process * cols;

    // Asignar memoria para almacenar las sumas locales de cada proceso
    int *localRowsDist = (int *)malloc(sizeof(int) * elementsPerProcess);
    int *localRowSums = (int *)malloc(sizeof(int) * rows_per_process);

    // Distribuir las filas de la matriz entre los procesos
    MPI_Scatterv(matrix, counts_send, displace_send, MPI_INT, localRowsDist, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);

    // Tiempo de inicio
    start_time = MPI_Wtime();

    // Calcular la suma de las filas locales
    int localSum;
    for (int i = 0; i < rows_per_process; i++)
    {
        localSum = 0;
        for (int j = 0; j < cols; j++)
        {
            localSum += localRowsDist[i * cols + j];
        }
        localRowSums[i] = localSum;
        // printf("La suma de la fila %d en el proceso %d es %d\n",i,rank,localRowSums[i] );
    }

    // Reunir el valor de la suma de cada fila en el proceso 0
    if (rank == 0)
    {
        rowSums = (int *)malloc(sizeof(int) * rows);
    }

    MPI_Gatherv(localRowSums, rows_per_process, MPI_INT, rowSums, counts_recv, displace_recv, MPI_INT, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();

    // Calcular el tiempo de ejecución en paralelo
    double execution_time = end_time - start_time;

    // Imprimir el vector resultante en el proceso 0
    if (rank == 0)
    {
        // printf("Suma de elementos de cada fila:\n");
        // for (int i = 0; i < rows; i++)
        // {
            // printf("Fila %d suma:%d\n", i, rowSums[i]);
        // }
        // printf("\n");
        
        // Imprimir el tiempo de ejecución en paralelo
        printf("Tiempo de ejecución: %.5f segundos\n", execution_time);
    }

    // Liberar memoria
    if (rank == 0)
    {
        free(matrix);
        free(rowSums);
        free(counts_send);
        free(displace_send);
        free(counts_recv);
        free(displace_recv);
    }
    free(localRowSums);
    free(localRowsDist);

    MPI_Finalize();
    return 0;
}