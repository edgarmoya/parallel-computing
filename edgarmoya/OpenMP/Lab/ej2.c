/* Implemente un programa paralelo utilizando OpenMP que calcule la media de los elementos
de un arreglo. Para inicializar el arreglo puede emplear la siguiente función y puede utilizar la
versión serie del cálculo de la media */

/* Edgar Moya Cáceres */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

// Función para inicializar un arreglo con números aleatorios
void randInit(int array[], const int n, const int bound) {
    srand(time(NULL));
    for (int i = 0; i < n; i++)
        array[i] = rand() % bound;
}

// Función para calcular la media de un arreglo
double mean(int array[], const int n) {
    double sum = 0;
    for (int i = 0; i < n; i++)
        sum += array[i];
    return (double)sum / n;
}

int main(int argc, char *argv[]) {
    int i;
    double parallel_sum = 0.0;
    double parallel_mean = 0.0;

    // Comprobar si se proporcionaron los argumentos requeridos
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tamaño del arreglo> <cantidad de hilos>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);          // Tamaño del arreglo
    int nthreads = atoi(argv[2]);   // Cantidad de hilos
    omp_set_num_threads(nthreads);

    // Reservar memoria para el arreglo
    int *array = (int *)malloc(sizeof(int) * n);

    // Inicializa el arreglo con números aleatorios con límite 100
    randInit(array, n, 100);

    // Iniciar tiempo de ejecución en paralelo
    double itime, ftime, exec_time; 
    itime = omp_get_wtime();

    // Realizar la suma en paralelo utilizando OpenMP
    #pragma omp parallel for private(i) shared(array, n) schedule(dynamic, 10000) reduction(+:parallel_sum)
    for (i = 0; i < n; i++) {
        parallel_sum += array[i];
    }

    // Resultados del tiempo de ejecución en paralelo
    ftime = omp_get_wtime(); 
    exec_time = ftime - itime; 
    printf("Tiempo de ejecución en paralelo: %.4f\n", exec_time);

    parallel_mean = parallel_sum / n;
    printf("Media calculada en paralelo: %.2f\n", parallel_mean);

    //*********//

    // Iniciar tiempo de ejecución en serie
    double time_spent = 0.0;
    clock_t begin = clock();

    // Calcular la media en serie
    double serial_mean = mean(array, n);

    // Resultados del tiempo de ejecución en serie
    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecución en serie: %.4f\n", time_spent);

    printf("Media calculada en serie: %.2f\n", serial_mean);

    // Liberar memoria
    free(array);

    return 0;
}
