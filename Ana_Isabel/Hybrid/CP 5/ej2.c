/******************************************************************************
Ana Isabel Portal Díaz

Ejercicio 2

Implemente un programa paralelo híbrido que permita calcular el producto escalar de
dos vectores.
A) Calcule la eficiencia y el speedup para distintos valores de longitud de los vectores

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>


void innit_vector(int *vector, const int cols, const int bound) {
    srand(time(NULL));
    for (int j = 0; j < cols; j++) {
        vector[j] = rand() % 10;
    }
}


int main(int argc, char *argv[])
{
    int rank, size, provided;
    int i, vector_size;

    int *vector_A, *vector_B;
    int *counts_ss_end_time, * displace_ss_end_time;
    int *local_A, *local_B;
    int remaining, elements_per_process;
    int local_size, local_start, local_s_end_time;
    int local_dot_product = 0, global_dot_product = 0;

    double SpeedUp,eficacia;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    
    if (provided != MPI_THREAD_FUNNELED){
        fprintf(stderr, "MPI not provided FUNNELED");
        exit(-1);
    }
    else {
        if (rank == 0)
            printf("MPI provided FUNNELED\n");
    }
    
    if (rank == 0){
        
        //Comprobar requerimientos 
        if (argc != 3) {
            printf("Uso: %s <cantidad de hilos> <tamaño de los vectores>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        
        // Obtener tamaño del vector
        if ((vector_size = atoi(argv[2])) == 0 ) {
            printf("El tamaño de los vectores debe ser un numero entero.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        // Asignar memoria para los vectores
        vector_A = (int *)malloc(vector_size * sizeof(int));
        vector_B = (int *)malloc(vector_size * sizeof(int));

        // Inicializar los vectores
        innit_vector(vector_A, vector_size, 10);
        innit_vector(vector_B, vector_size, 10);

        
        
        counts_ss_end_time = (int *)malloc(sizeof(int) * size);
        displace_ss_end_time = (int *)malloc(sizeof(int) * size);

        remaining = vector_size % size;

        elements_per_process = vector_size / size;
         
        displace_ss_end_time[0] = 0;

        for (int i = 0, j = remaining; i < size; i++, j--){            
            counts_ss_end_time[i] = elements_per_process;
            if (j > 0){
                counts_ss_end_time[i] += 1;
            }            
            if (i > 0){
                displace_ss_end_time[i] = displace_ss_end_time[i - 1] + counts_ss_end_time[i - 1];
            }
        }
    }
    
    // Difundir el tamaño de los vectores a todos los procesos
    MPI_Bcast(&vector_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular tiempo de ejecución en paralelo
    double p_initial_time, p_final_time, p_time;

    elements_per_process = vector_size / size;

    remaining = vector_size % size;

    if (rank < remaining){
        elements_per_process++;
    }        

    local_A = (int *)malloc(sizeof(int) * elements_per_process);
    local_B = (int *)malloc(sizeof(int) * elements_per_process);
    
    // Configurar la cantidad de hilos a utilizar
    omp_set_num_threads(atoi(argv[1]));

    #pragma omp parallel
    {
        #pragma omp barrier
        #pragma omp master
        {
            // Distribuir los vectores
            MPI_Scatterv(vector_A, counts_ss_end_time, displace_ss_end_time, MPI_INT, local_A, elements_per_process, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Scatterv(vector_B, counts_ss_end_time, displace_ss_end_time, MPI_INT, local_B, elements_per_process, MPI_INT, 0, MPI_COMM_WORLD);
        }
        #pragma omp barrier
        
        p_initial_time = MPI_Wtime();
        // Calcular el producto escalar local en paralelo
        #pragma omp for private(i) reduction(+ : local_dot_product)
            for (i = 0; i < elements_per_process; i++)
            {
                local_dot_product += local_A[i] * local_B[i];
            }
        
        // Sumar los productos escalares locales en el proceso 0
        #pragma omp barrier
        #pragma omp master
        {
            MPI_Reduce(&local_dot_product, &global_dot_product, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        }
        #pragma omp barrier

        p_final_time = MPI_Wtime();

    }
    
    // Imprimir el producto escalar resultante en el proceso 0
    if (rank == 0){

        printf("Producto escalar: %d\n", global_dot_product);
        p_time = p_final_time - p_initial_time; 

        printf("Tiempo de ejecución en paralelo: %.4f\n", p_time);

        /********************
        Ejecucion secuencial
        ********************/
        double s_time = 0.0;
        clock_t s_start_time = clock();

        int prod_esc = 0;

        for(i = 0; i < vector_size; i++){
            prod_esc += vector_A[i]*vector_B[i];
        }
    
        // Resultados del tiempo de ejecución en paralelo
        clock_t s_end_time = clock();
        s_time += (double)(s_end_time - s_start_time) / CLOCKS_PER_SEC;
        
        printf("Producto escalar: %d\n", prod_esc);
        printf("Tiempo de ejecución en serie: %.4f\n", s_time);

        SpeedUp = s_time / p_time;
        printf("El speedup (St/Sp) es: %.6f\n", SpeedUp);

        eficacia = SpeedUp / size;
        printf("La eficacia es: %.6f\n", eficacia);
        
    }
        
    
    if(rank == 0){
        free(counts_ss_end_time);
        free(displace_ss_end_time);
        free(vector_A);
        free(vector_B);
    }

    free(local_A);
    free(local_B);
    
    MPI_Finalize();

    return 0;
}