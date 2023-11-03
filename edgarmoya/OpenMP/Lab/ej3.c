#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

#define N 1000 // Tamaño de la matriz y el vector

// Función para imprimir una matriz
void print_matrix(double matriz[][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%.1f ", matriz[i][j]);
        }
        printf("\n");
    }
}

// Función para imprimir un vector
void print_vector(double vector[]) {
    for (int i = 0; i < N; i++) {
        printf("%.1f ", vector[i]);
    }
    printf("\n");
}

void prodmv(double a[N], double c[N], double B[N][N]){
    int i, j;
    double sum;
    for (i = 0; i < N; i++) {
        sum=0;
        for(j=0;j<N;j++)
            sum += B[i][j] * c[j];
        a[i] = sum;
    }
}

int main() {
    int i, j;
    double sum;
    double a[N], c[N];
    double B[N][N];

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
    prodmv(aa, c, B);
 
    // Resultados del tiempo de ejecución en paralelo
    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
 
    printf("Tiempo de ejecución en serie: %.4f\n", time_spent);

    // El resultado estará en el arreglo 'a'
    // print_matrix(B);
    // print_vector(c);
    // print_vector(a);
    // print_vector(aa);

    return 0;
}
