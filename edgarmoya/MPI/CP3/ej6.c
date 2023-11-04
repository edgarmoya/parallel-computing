/* En un programa paralelo ejecutado en p procesos, se tiene un vector x de dimensión n 
distribuido por bloques, y un vector y replicado en todos los procesos. Implementar la 
siguiente función, la cual debe sumar la parte local del vector x con la parte correspondiente
del vector y, dejando el resultado en un vector local z.
void suma(double xloc[], double y[], double z[], int n, int p, int pr)
- pr es el indice del proceso local */

/* Edgar Moya Cáceres */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

void suma(double xloc[], double y[], double z[], int n, int p, int pr) {
    // Calcular la cantidad de elementos por bloque
    int elementos_x_bloque = n / p;
    
    // Calcular el índice de inicio y fin de la parte local de "x" para este proceso
    int inicio_local = pr * elementos_x_bloque;
    int fin_local = inicio_local + elementos_x_bloque;
    
    // Sumar la parte local de "x" con la parte correspondiente de "y" y almacenar en "z"
    for (int i = inicio_local; i < fin_local; i++) {
        z[i - inicio_local] = xloc[i - inicio_local] + y[i];
    }
}

double random_value(double min, double max){
    double r = (double)rand() / RAND_MAX;
    return min + r * (max - min);
}

int main(int argc, char **argv) {
    int rank, size;
    int elements_x_proc;
    double *x = NULL;
    double *local_x = NULL;
    double *y = NULL;
    double *z = NULL;
    int N;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Longitud del vector: \n");
        scanf("%d", &N);

        // Comprobar que el tamaño es múltiplo de la cantidad de procesos
        if (N % size != 0) {
            printf("La longitud debe ser múltiplo de la cantidad de procesos.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        srand(time(NULL));

        // Generar un vector "x" con números aleatorios
        x = (double *)malloc(N * sizeof(double));
        for (int i = 0; i < N; i++){
            x[i] = random_value(1.0, 10.0);
        }
        
        // Generar un vector "y" con números aleatorios
        y = (double *)malloc(N * sizeof(double));
        for (int i = 0; i < N; i++){
            y[i] = random_value(1.0, 10.0);
        }

        // Imprimir vectores
        printf("Vector inicial x: \n");
        for (int i = 0; i < N; i++){
            printf("%.2lf ", x[i]);
        }
        printf("\n");

        printf("Vector inicial y: \n");
        for (int i = 0; i < N; i++){
            printf("%.2lf ", y[i]);
        }
        printf("\n");
    }

    // Difundir el tamaño del vector a todos los procesos
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0){
        y = (double *)malloc(N * sizeof(double));
    }

    // Difundir el vector "y" a todos los procesos
    MPI_Bcast(y, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Cantidad de elementos por cada proceso
    elements_x_proc = N / size;

    // Distribuir el vector "x" por cada proceso
    local_x = (double *)malloc(elements_x_proc * sizeof(double));
    MPI_Scatter(x, elements_x_proc, MPI_DOUBLE, local_x, elements_x_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Calcular la suma de cada porción del vector
    z = (double *)malloc(elements_x_proc * sizeof(double));
    suma(local_x, y, z, N, size, rank);

    // Imprimir resultados
    printf("Vector local z del proceso %d: \n", rank);
    for (int i = 0; i < elements_x_proc; i++){
        printf("%.2lf ", z[i]);
    }
    printf("\n");

    // Liberar memoria
    if (rank == 0){
        free(x);
        free(y);
    }
    free(local_x);
    free(z);

    MPI_Finalize();
    return 0;
}
