/* Programa paralelo híbrido que permite calcular el producto escalar de dos 
vectores. Se asume que el tamaño de los vectores es múltiplo de la cantidad 
de procesos. Se implementó el programa secuencial para calcular el speedup */

/* Edgar Moya Cáceres */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    int rank, size, vector_size, i, provided;
    int *vector_a, *vector_b;
    int *vector_local_a, *vector_local_b;
    int local_sum = 0, global_sum = 0;
    double parallel_start_time, parallel_end_time;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if(provided != MPI_THREAD_FUNNELED) {
        fprintf(stderr, "MPI not provide FUNNELED\n");
        exit(-1);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Comprobar si se proporcionaron los argumentos requeridos
        if (argc != 3) {
            printf("Uso: %s <tamaño del vector> <cantidad de hilos>\n", argv[0]);
            exit(-1);
        }

        // Obtener tamaño del vector
        if ((vector_size = atoi(argv[1])) == 0) {
            printf("El tamaño del vector debe ser un número entero.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        // Comprobar que el tamaño del vector es múltiplo de la cantidad de procesos
        if (vector_size % size != 0) {
            printf("El tamaño del vector debe ser un múltiplo del número de procesos.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        // Asignación de memoria para los vectores
        vector_a = (int *)malloc(vector_size * sizeof(int));
        vector_b = (int *)malloc(vector_size * sizeof(int));

        // Inicializar los vectores con valores aleatorios
        srand(time(NULL));
        for (int i = 0; i < vector_size; i++) {
            vector_a[i] = rand() % 10;
        }
        for (int j = 0; j < vector_size; j++) {
            vector_b[j] = rand() % 10;
        }
    }

    // Difundir el tamaño de los vectores a todos los procesos
    MPI_Bcast(&vector_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Configurar la cantidad de hilos a utilizar
    omp_set_num_threads(atoi(argv[2]));

    #pragma omp parallel shared(vector_size, size, vector_local_a, vector_local_b), \
            shared(ompi_mpi_comm_world, ompi_mpi_int)
    {
        #pragma omp barrier
        #pragma omp master
        {
             // Asignar memoria solo en el hilo maestro y distribuir los vectores a todos los procesos
            vector_local_a = (int *)malloc(vector_size/size * sizeof(int));
            MPI_Scatter(vector_a, vector_size/size, MPI_INT, vector_local_a, vector_size/size, MPI_INT, 0, MPI_COMM_WORLD);

            vector_local_b = (int *)malloc(vector_size/size * sizeof(int));
            MPI_Scatter(vector_b, vector_size/size, MPI_INT, vector_local_b, vector_size/size, MPI_INT, 0, MPI_COMM_WORLD);
        }
        #pragma omp barrier

        // Tiempo de inicio
        parallel_start_time = MPI_Wtime();
        
        // Calcular el producto escalar local en paralelo
        #pragma omp for private(i) reduction(+:local_sum)
            for (i = 0; i < vector_size/size; i++) {
                local_sum += vector_local_a[i] * vector_local_b[i];
            }
        
        // Sumar los productos escalares locales en el proceso 0
        #pragma omp barrier
        #pragma omp master
        MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        
        #pragma omp barrier
    }

    // Tiempo de fin
    parallel_end_time = MPI_Wtime();

    // Imprimir el tiempo de ejecución en paralelo y el resultado del producto escalar
    if (rank == 0){
        printf("Tiempo de ejecución (paralelo): %.6f seg\n", parallel_end_time - parallel_start_time);
        printf("Producto escalar (paralelo): %d\n", global_sum);
    }

    // Liberar la memoria
    free(vector_local_a);
    free(vector_local_b);


    /***** SECUENCIAL *****/
    
    if (rank == 0) {
        int sum = 0;

        // Medir el tiempo antes de calcular
        clock_t sequential_start_time = clock();

        // Calcular producto escalar
        for (int i = 0; i < vector_size; i++) {
            sum += vector_a[i] * vector_b[i];
        }

        // Medir el tiempo después de calcular
        clock_t sequential_end_time = clock();

        // Imprimir el tiempo de ejecución secuencial y el resultado del producto escalar
        printf("Tiempo de ejecución (secuencial): %.6f seg\n", (double)(sequential_end_time-sequential_start_time) / CLOCKS_PER_SEC);
        printf("Producto escalar (secuencial): %d\n", sum);

        // Liberar la memoria
        free(vector_a);
        free(vector_b);
    }

    MPI_Finalize();
    return 0;
}
