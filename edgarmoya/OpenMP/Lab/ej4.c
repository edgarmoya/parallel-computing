#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define N 100   // Tamaño de la matriz

void print_matrix(int matrix[][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int matrix[N][N];
    omp_set_num_threads(10);

    // Inicializar matriz
    srand(time(NULL));
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            matrix[i][j] = rand() % 10;

    int i, j, min, max;
    min = matrix[0][0];
    max = matrix[0][0];

    #pragma omp parallel for private(j)
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            #pragma omp critical
            {
                if (matrix[i][j] < min) {
                    min = matrix[i][j];
                }
                if (matrix[i][j] > max) {
                    max = matrix[i][j];
                }
            }
        }
    }

    print_matrix(matrix);
    printf("El menor elemento de la matriz es: %d\n", min);
    printf("El mayor elemento de la matriz es: %d\n", max);

    return 0;
}
