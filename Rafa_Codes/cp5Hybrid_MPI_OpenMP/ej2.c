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
    for (int i = 0; i < n; i++)
        array[i] = rand() % bound;
}


int main(int argc, char *argv[])
{
    int rank, size, provide;
    int i, v_size;
    int *vectorA, *vectorB, *counts_send, * displace_send, *localA, *localB;
    int remaining, elementsPerProcess, localSize, localStart, localEnd;
    int localDotProduct = 0, globalDotProduct = 0;

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

        if (argc != 2) {
            printf("Uso: %s <tamnno de los vectores>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        
        if ((v_size = atoi(argv[1])) == 0 ) {
            printf("El tamanno de los vectores debe ser un numero entero.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        srand(time(NULL));
        vectorA = (int *)malloc(v_size * sizeof(int));
        vectorB = (int *)malloc(v_size * sizeof(int));

        randInit(vectorA, v_size, 10);
        randInit(vectorB, v_size, 10);

        // printf("Los vectores quedan con la forma:\n A:");

        // for (int i = 0; i < v_size; i++)
        // {
        //     printf("%d ", vectorA[i]);
        // }
        //     printf("\n B:");
        
        // for (int i = 0; i < v_size; i++)
        // {
        //     printf("%d ", vectorB[i]);
        // }
        //     printf("\n");
        
        counts_send = (int *)malloc(sizeof(int) * size);
        displace_send = (int *)malloc(sizeof(int) * size);
        remaining = v_size % size;
        elementsPerProcess = v_size / size;
         
        displace_send[0] = 0;
        for (int i = 0, j = remaining; i < size; i++, j--)
        {
            counts_send[i] = elementsPerProcess;

            if (j > 0)
            {
                counts_send[i] += 1;
            }
            if (i > 0)
            {
                displace_send[i] = displace_send[i - 1] + counts_send[i - 1];
            }
        }
    }
    
    MPI_Bcast(&v_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular tiempo de ejecución
    double itime, ftime, exec_time;

    elementsPerProcess = v_size / size;
    remaining = v_size % size;

    if (rank < remaining)
    {
        elementsPerProcess++;
    }        

    localA = (int *)malloc(sizeof(int) * elementsPerProcess);
    localB = (int *)malloc(sizeof(int) * elementsPerProcess);
    
    omp_set_num_threads(8);

    #pragma omp parallel
    {
        #pragma omp barrier
        #pragma omp master
        {
            // Distribuir los vectores entre los procesos
            MPI_Scatterv(vectorA, counts_send, displace_send, MPI_INT, localA, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Scatterv(vectorB, counts_send, displace_send, MPI_INT, localB, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);
        }
        #pragma omp barrier
        
        itime = MPI_Wtime();
        // Calcular el producto escalar local en paralelo
        #pragma omp for private(i) reduction(+ : localDotProduct)
            for (i = 0; i < elementsPerProcess; i++)
            {
                localDotProduct += localA[i] * localB[i];
            }
        
        // Sumar los productos escalares locales en el proceso 0
        #pragma omp barrier
        #pragma omp master
        {
            MPI_Reduce(&localDotProduct, &globalDotProduct, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        }
        #pragma omp barrier

        ftime = MPI_Wtime();

    }
    
    // Imprimir el producto escalar resultante en el proceso 0
    if (rank == 0)
    {
        printf("Producto escalar: %d\n", globalDotProduct);
        exec_time = ftime - itime; 
        printf("Tiempo de ejecución en paralelo: %.4f\n", exec_time);

        //Ejecucion secuencial:
        double time_spent = 0.0;
        clock_t begin = clock();

        int prod_esc = 0;

        for(i = 0; i < v_size; i++)
        {
            prod_esc += vectorA[i]*vectorB[i];
        }
    
        // Resultados del tiempo de ejecución en paralelo
        clock_t end = clock();
        time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
        
        printf("Producto escalar: %d\n", prod_esc);
        printf("Tiempo de ejecución en serie: %.4f\n", time_spent);

    }
        
    
    if(rank == 0){
        free(counts_send);
        free(displace_send);
        free(vectorA);
        free(vectorB);

    }
    free(localA);
    free(localB);
    
    MPI_Finalize();

    return 0;
}