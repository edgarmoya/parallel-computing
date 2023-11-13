/******************************************************************************
Ana Isabel Portal Diaz

Ejercicio 4
Implemente un programa paralelo con OpenMP que permita determinar el menor y mayor
elemento de una matriz

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// Funcion para imprimir la matriz 
void print_matrix(double **matriz, int N, int M) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            printf("%f ", matriz[i][j]);
        }
        printf("\n");
    }
}

// Funcion para inicializar un arreglo de dos dimensiones no cuadratico
void randArray2_Not_Cuadratic(double **array, const int n,  const int m, const int bound) {
    srand(time(NULL));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            array[i][j] = rand() % bound;
}

int main(int argc, char *argv[]){
    double min, max;
    int i, j, N, M;

    if (argc != 3) {
        printf("Uso: %s <total de filas de la matriz> <total de columnas de la matriz>\n", argv[0]);
        return 0;
    }

    if ((N = atoi(argv[1])) == 0 || (M = atoi(argv[2])) == 0) {
        printf("La cantidad de filas y de columnsa de la matriz deben ser números enteros.\n");
        return 0;
    }

    double **matrix = malloc(sizeof(double) * N);    
    for(i = 0; i < N; i++){
        matrix[i] = malloc(sizeof(double) * M);
    }

    omp_set_num_threads(10);

    //Inicializar la matriz
    randArray2_Not_Cuadratic(matrix,N,M,10);

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

    print_matrix(matrix,N,M);
    printf("El menor elemento de la matriz es: %f\n", min);
    printf("El mayor elemento de la matriz es: %f\n", max);
   

}