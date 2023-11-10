/*
Multiplicar un vector de tamaño N por una matriz de tamaño NxN
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

// Función para imprimir una matriz
void print_matrix(double **matriz, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%.1f ", matriz[i][j]);
        }
        printf("\n");
    }
}

// Función para imprimir un vector
void print_vector(double *vector, int N) {
    for (int i = 0; i < N; i++) {
        printf("%.1f ", vector[i]);
    }
    printf("\n");
}

void prodmv(double *a, double *c, double **B, int N){
    int i, j;
    double sum;
    for (i = 0; i < N; i++) {
        sum=0;
        for(j=0;j<N;j++)
            sum += B[i][j] * c[j];
        a[i] = sum;
    }
}

int main(int argc, char *argv[]) {
    int i, j;
    double sum;

    int N;    // Tamaño del arreglo    

    if (argc != 2) {
            printf("Uso: %s <tamaño de la matriz y del vector>\n", argv[0]);
            return 0;
    }

    if ((N = atoi(argv[1])) == 0 ) {
        printf("El tamaño del vector y de la matriz deben ser números enteros.\n");
        return 0;
    }

    double *a = malloc(sizeof(double) * N);
    double *c = malloc(sizeof(double) * N);
    double **B = malloc(sizeof(double) * N);
    
    for(i = 0; i < N; i++){
        B[i] = malloc(sizeof(double) * N);
    }

    omp_set_num_threads(10);

    // Inicializar c y B
    srand(time(NULL));
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            B[i][j] = rand() % 10;
    
    for (int i = 0; i < N; i++)
        c[i] = rand() % 10;

    // Calcular tiempo de ejecución
    double itime, ftime, exec_time; 
    itime = omp_get_wtime();

    #pragma omp parallel for private(j, sum)
        for (i = 0; i < N; i++) {
            sum = 0;
            for (j = 0; j < N; j++) {
                sum += B[i][j] * c[j];
            }
            a[i] = sum;
        }

    // Resultados del tiempo de ejecución en paralelo
    ftime = omp_get_wtime(); 
    exec_time = ftime - itime; 
    printf("Tiempo de ejecución en paralelo: %.4f\n", exec_time);


    // Calcula la media en serie para comparar
    double time_spent = 0.0;
    clock_t begin = clock();

    double aa[N];
    prodmv(aa, c, B, N);
 
    // Resultados del tiempo de ejecución en paralelo
    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
 
    printf("Tiempo de ejecución en serie: %.4f\n", time_spent);

    // El resultado estará en el arreglo 'a'
    // print_matrix(B,N);
    // print_vector(c,N);
    // print_vector(a,N);
    // print_vector(aa,N);

    free(B);
    free(a);
    free(c);

    return 0;
}
