/*
Implemente un programa paralelo utilizando OpenMP que calcule la media de los elementos
de un arreglo. Para inicializar el arreglo puede emplear la siguiente función y puede utilizar la
versión serie del cálculo de la media
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

void randInit(int array[], const int n, const int bound) {
    srand(time(NULL));
    for (int i = 0; i < n; i++)
        array[i] = rand() % bound;
}

double mean(int array[], const int n) {
    // funcion serie
    double sum = 0;
    for (int i = 0; i < n; i++)
        sum += array[i];
    return (double)sum / n;

    // funcion paralelizada
    // double parallel_sum = 0.0;
    // #pragma omp parallel for reduction(+:parallel_sum)
    // for (int i = 0; i < n; i++) {
    //     parallel_sum += array[i];
    // }
    // return parallel_sum/n;    
}

int main(int argc, char *argv[]) {
    
    int n;    // Tamaño del arreglo    
    int bound;    // Valor máximo de los elementos

    if (argc != 3) {
            printf("Uso: %s <tamaño del arreglo> <cota de elementos>\n", argv[0]);
            return 0;
    }

    if ((n = atoi(argv[1])) == 0 || (bound = atoi(argv[2])) == 0) {
        printf("El tamaño del arreglo y la cota de elementos deben ser números enteros.\n");
        return 0;
    }

    omp_set_num_threads(10);

    int *array = malloc(sizeof(int)*n);
   
    randInit(array, n, bound);

    double parallel_sum = 0.0;
    double parallel_mean = 0.0;

    // Calcular tiempo de ejecución
    double itime, ftime, exec_time; 
    itime = omp_get_wtime();

    #pragma omp parallel for reduction(+:parallel_sum)
    for (int i = 0; i < n; i++) {
        parallel_sum += array[i];
    }

    parallel_mean = parallel_sum / n;
    printf("Media calculada en paralelo: %.2f\n", parallel_mean);

    // Resultados del tiempo de ejecución en paralelo
    ftime = omp_get_wtime(); 
    exec_time = ftime - itime; 
    printf("Tiempo de ejecución en paralelo: %.4f\n", exec_time);

    // Calcula la media en serie para comparar
    double time_spent = 0.0;
    clock_t begin = clock();

    double serial_mean = mean(array, n);
    printf("Media calculada en serie: %.2f\n", serial_mean);
 
    // Resultados del tiempo de ejecución en paralelo
    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
 
    printf("Tiempo de ejecución en serie: %.4f\n", time_spent);

    free(array);

    return 0;
}
