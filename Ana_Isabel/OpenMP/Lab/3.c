/******************************************************************************
Ana Isabel Portal Diaz

Ejercicio 3
El siguiente código secuencial implementa el producto de una matriz B de dimensión
N x N por un vector c de dimension N

void prodmv(double a[N], double c[N], double B[N][N] )
{
    int i, j; 
    double sum;
        for (i = 0; i < N; i++) {
            sum=0;
            for(j=0;j<N;j++)
                sum += B[i][j]*c[j];
            a[i]=sum;
        }
}

A) Escriba un programa paralelo con OpenMP del código dado
B) Calcule el speedup y la eficiencia

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>


// Funcion para ejecutar el producto de una amtriz de dimension
// N x N por un vector de dimension N
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

// Función para imprimir una matriz
void print_matrix(double **matriz, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%.1f ", matriz[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Función para imprimir un vector
void print_vector(double *vector, int N) {
    for (int i = 0; i < N; i++) {
        printf("%.1f ", vector[i]);
    }
    printf("\n");
}

// Funcion para inicializar un arreglo de dos dimensiones cuadratico
void randArray2(double** array, const int n, const int bound) {
    srand(time(NULL));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            array[i][j] = rand() % bound;
}

// Funcion para inicializar un arreglo de una dimension 
void randArray1(double array[], const int n, const int bound) {
  srand(time(NULL));
  for (int i = 0; i < n; i++)
    array[i] = rand() % bound;
}

int main (int argc, char *argv[]) {
    // double sum;
    // int nthreads, tid, i, j;
    // //Vector
    // int *a=NULL;
    // a=(int*) malloc(N *sizeof(int));

    // int **B=NULL;
    // double *B=NULL;
    // B=(double**) malloc(N *sizeof(double));

    // for (int i = 0; i < N; i++) {
    //   a[i]=(double*) malloc(N *sizeof(double));
    //   }

    // randInit(a,N,100);
    // int total;
    //     #pragma omp parallel for shared(nthreads,a,sum) private(i,j,tid) reduction(+:sum)
    //     {
    //         for (i = 0; i < N; i++) {
    //             sum=0;
    //             for(j=0;j<N;j++)
    //                 sum += B[i][j]*c[j];
    //             a[i]=sum;
    //         }

    //     }

    double sum;
    int i, j, nthreads; // indices, cantidad de hilos
    int N;    //Tamgno del arreglo
        
    if (argc != 3) {
        printf("Uso: %s <tamaño de la matriz y del vector> <cantidad de hilos>\n", argv[0]);
        return 0;
    }

    if ((N = atoi(argv[1])) == 0 ) {
        printf("El tamaño del vector y de la matriz deben ser números enteros.\n");
        return 0;
    }

    nthreads = atoi(argv[2]);

    double *a = malloc(sizeof(double) * N);
    double *c = malloc(sizeof(double) * N);

    double **B = malloc(sizeof(double) * N);
    for(i = 0; i < N; i++){
        B[i] = malloc(sizeof(double) * N);
    }

    omp_set_num_threads(nthreads);

    // Inicializar c y B
    randArray1(c,N,10);
    randArray2(B,N,10);

    // Calcular tiempo de ejecución
    double inicial_time, final_time, exec_time; 
    inicial_time = omp_get_wtime();   

    #pragma omp parallel for private(j, sum)
        for (i = 0; i < N; i++) {
            sum = 0;
            for (j = 0; j < N; j++) {
                sum += B[i][j] * c[j];
            }
            a[i] = sum;
        }
    // Resultados del tiempo de ejecución en paralelo
    final_time = omp_get_wtime(); 
    exec_time = final_time - inicial_time; 
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
    // printf("\n La matriz B es: \n");
    // print_matrix(B,N);
    // printf("\n El c vector es: \n");
    // print_vector(c,N);
    // printf("\n El a vector es: \n");
    // print_vector(a,N);
    // printf("\n El vector aa es: \n");
    // print_vector(aa,N);

    //Liberar memoria
    free(B);
    free(a);
    free(c);

    return 0;
}