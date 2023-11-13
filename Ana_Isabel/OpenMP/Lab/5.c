/******************************************************************************
Ana Isabel Portal Diaz

Ejercicio 5
Desarrolle un programa paralelo que permita determinar la cantidad de
números primos en un rango de [1..N]. Puede basarse en la siguiente 
función:

Objetivo:
    retorna la cantidad de primos entre 1 y N.
Parámetros:
    Input, int N, valor máximo a chequear.
    Output, int PRIME_NUMBER, cantidad de primos

int prime_number ( int n ){
    int i;
    int j;
    int prime;
    int total = 0;
    for ( i = 2; i <= n; i++ ){
        prime = 1;
        for ( j = 2; j < i; j++ ){
            if ( i % j == 0 ){
                prime = 0;
                break;
            }
        }
        total = total + prime;
    }
    return total;
}

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[]){
    int N;  // Numero hasta el cual buscaremos numeros primos
    int i, j, prime, total = 0;
    omp_set_num_threads(10); 

    if (argc != 2) {
        printf("Tiene que introducir hasta que numero sebuscaran la cantidad de nuemros primos");
        return 0;
    }

    N = atoi(argv[1]);

    #pragma omp parallel for private(j, prime) reduction(+:total)
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
