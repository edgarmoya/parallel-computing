/* Implemente un programa paralelo con OpenMP que permita determinar el menor y mayor
elemento de una matriz */

/* Edgar Moya Cáceres */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// Función para imprimir una matriz (esta función no se usa en el código actual)
void print_matrix(int **matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int i, j, min, max;

    // Comprobar si se proporcionaron los argumentos requeridos
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tamaño de la matriz> <cantidad de hilos>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);          // Tamaño de la matriz
    int nthreads = atoi(argv[2]);   // Cantidad de hilos

    // Configurar la cantidad de hilos a utilizar con OpenMP
    omp_set_num_threads(nthreads);

    // Reservar memoria para la matriz
    int **matrix = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++){
        matrix[i] = (int *)malloc(N * sizeof(int));
    }

    // Inicializar matriz con valores aleatorios
    srand(time(NULL));
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            matrix[i][j] = rand() % 100;

    // Inicializar min y max con el primer elemento de la matriz
    min = matrix[0][0];
    max = matrix[0][0];

    // Comienza una sección paralela con OpenMP para buscar el mínimo y el máximo
    #pragma omp parallel for shared(N, matrix, min, max) private(i, j)
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            // Comienza una región crítica
            #pragma omp critical
            {
                // Comprobar si el elemento en la posición (i, j) es menor que el mínimo actual
                if (matrix[i][j] < min) {
                    min = matrix[i][j];   // Actualiza el valor mínimo
                }
                // Comprobar si el elemento en la posición (i, j) es mayor que el máximo actual
                if (matrix[i][j] > max) {
                    max = matrix[i][j];   // Actualiza el valor máximo
                }
            }
        }
    }

    // print_matrix(matrix, N);
    printf("El menor elemento de la matriz es: %d\n", min);
    printf("El mayor elemento de la matriz es: %d\n", max);

    // Liberar memoria
    for (int i = 0; i < N; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}
