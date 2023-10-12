/*
   Rafael_Fernandez_Fleites
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

void suma(double x_loc[], double y[], double z[], int n, int p, int pr)
{

    // Cantidad de elementos por bloque
    int elementsPerProcess = n / p;

    // Iinicio y fin de la parte local de x para este proceso
    int local_start = pr * elementsPerProcess;
    int local_end = local_start + elementsPerProcess;

    // Sumar el pedazo de x con su parte correspondiente de y y guardar en z
    for (int i = local_start; i < local_end; i++)
    {
        z[i - local_start] = x_loc[i - local_start] + y[i];
    }
}

double generarAleatorioDouble(double min, double max)
{
    double r = (double)rand() / RAND_MAX; // Generar número aleatorio entre 0 y 1
    return min + r * (max - min);         // Escalar el número aleatorio al rango deseado
}

int main(int argc, char **argv)
{
    int rank, size;
    int elementsPerProcess;
    double *x;
    double *local_x;
    double *y;
    double *z;
    int N;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        printf("Tamaño del vector: \n");
        scanf("%d", &N);

        if (N % size != 0)
        {
            printf("La longitud del vector debe ser múltiplo de la cantidad de procesos.\n");
            MPI_Finalize();
            return 0;
        }

        srand(time(NULL));

        // Generar un vector x con números aleatorios
        x = (double *)malloc(N * sizeof(double));
        for (int i = 0; i < N; i++)
        {
            x[i] = generarAleatorioDouble(-10.0, 10.0);
        }

        // Generar un vector "y" con números aleatorios
        y = (double *)malloc(N * sizeof(double));
        for (int i = 0; i < N; i++)
        {
            y[i] = generarAleatorioDouble(-10.0, 10.0);
        }

        // Imprimir vectores
        printf("Vector inicial x: \n");
        for (int i = 0; i < N; i++)
        {
            printf("%.2lf ", x[i]);
        }
        printf("\n");

        printf("Vector inicial y: \n");
        for (int i = 0; i < N; i++)
        {
            printf("%.2lf ", y[i]);
        }
        printf("\n");
    }

    // Difundir el tamaño del vector a todos los procesos
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0)
    {
        y = (double *)malloc(N * sizeof(double));
    }

    // Difundir el vector y a todos los procesos
    MPI_Bcast(y, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Cantidad de elementos por cada proceso
    elementsPerProcess = N / size;

    // Distribuir el vector x por cada proceso
    local_x = (double *)malloc(elementsPerProcess * sizeof(double));
    MPI_Scatter(x, elementsPerProcess, MPI_DOUBLE, local_x, elementsPerProcess, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Calcular la suma de cada porción del vector
    z = (double *)malloc(elementsPerProcess * sizeof(double));
    suma(local_x, y, z, N, size, rank);

    // Imprimir resultados
    printf("Vector local z del proceso %d: \n", rank);
    for (int i = 0; i < elementsPerProcess; i++)
    {
        printf("%.2lf ", z[i]);
    }
    printf("\n");

    // Liberar memoria
    if (rank == 0)
    {
        free(x);
    }
    free(y);
    free(local_x);
    free(z);

    return 0;
}
