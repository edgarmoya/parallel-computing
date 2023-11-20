/* Implementar el producto de una matriz B de dimensión N x N por un 
vector c de dimensión N */

/* Edgar Moya Cáceres */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

// Función para imprimir una matriz
/*void print_matrix(double **matriz, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%.1f ", matriz[i][j]);
        }
        printf("\n");
    }
}*/

// Función para imprimir un vector
/*void print_vector(double *vector, int size) {
    for (int i = 0; i < size; i++) {
        printf("%.1f ", vector[i]);
    }
    printf("\n");
}*/

// Función para realizar el producto de una matriz B por un vector c
void prodmv(double *a, double *c, double **B, int size) {
    for (int i = 0; i < size; i++) {
        a[i] = 0.0;
        for (int j = 0; j < size; j++) {
            a[i] += B[i][j] * c[j];
        }
    }
}

// Función para generar un número aleatorio en un rango específico
double random_value(double min, double max){
    double r = (double)rand() / RAND_MAX;
    return min + r * (max - min);
}

int main(int argc, char *argv[]) {
    int i, j;
    double itime, ftime, exec_time;

    // Comprobar si se proporcionaron los argumentos requeridos
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tamaño> <cantidad de hilos>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);          // Tamaño
    int nthreads = atoi(argv[2]);   // Cantidad de hilos

    // Configura la cantidad de hilos a utilizar con OpenMP
    omp_set_num_threads(nthreads);

    // Reservar memoria para la matriz y los vectores
    double *a = (double *)malloc(sizeof(double) * N);
    double *c = (double *)malloc(sizeof(double) * N);

    double **B = (double **)malloc(N * sizeof(double *));
    for (int i = 0; i < N; i++){
        B[i] = (double *)malloc(N * sizeof(double));
    }

    // Inicializar c y B con valores aleatorios
    srand(time(NULL));
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            B[i][j] = random_value(1.0, 10.0);
    
    for (int i = 0; i < N; i++)
        c[i] = random_value(1.0, 10.0);

    // Iniciar tiempo de ejecución en paralelo
    itime = omp_get_wtime();

    #pragma omp parallel for private(i, j) shared(a, c, B, N)
        for (i = 0; i < N; i++) {
            a[i] = 0.0;
            for (j = 0; j < N; j++) {
                a[i] += B[i][j] * c[j];
            }
        }

    // Resultados del tiempo de ejecución en paralelo
    ftime = omp_get_wtime(); 
    exec_time = ftime - itime;
    printf("Tiempo de ejecución en paralelo: %.4f\n", exec_time);
    
    //*********//

    // Iniciar tiempo de ejecución en serie
    double time_spent = 0.0;
    clock_t begin = clock();

    // Calcula la media en serie para comparar
    double *a_serie = (double *)malloc(sizeof(double) * N);
    prodmv(a_serie, c, B, N);

    // Resultados del tiempo de ejecución en serie
    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecución en serie: %.4f\n", time_spent);

    // Liberar memoria
    free(a);
    free(c);
    for (int i = 0; i < N; i++) {
        free(B[i]);
    }
    free(B);
    free(a_serie);
    
    return 0;
}
