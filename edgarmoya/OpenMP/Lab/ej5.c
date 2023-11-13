/* Desarrolle un programa paralelo que permita determinar la cantidad de números primos en
un rango de [1..N] */

/* Edgar Moya Cáceres */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    int i, j, prime, total = 0;

    // Comprobar si se proporcionaron los argumentos requeridos
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <rango> <cantidad de hilos>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);          // Rango
    int nthreads = atoi(argv[2]);   // Cantidad de hilos

    // Configurar la cantidad de hilos a utilizar con OpenMP
    omp_set_num_threads(nthreads);

    #pragma omp parallel for private(i, j, prime) shared(N) reduction(+:total)
    for (i = 2; i <= N; i++) {
        prime = 1;
        for (j = 2; j < i; j++) {
            if (i % j == 0) {
                prime = 0;
                break;
            }
        }
        total += prime;
    }

    printf("Cantidad de números primos entre 1 y %d: %d\n", N, total);

    return 0;
}
