/* Modifique el programa anterior para en lugar de calcular la suma se obtenga el 
menor elemento del vector. a) Posición de dicho elemento */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <mpi.h>

typedef struct {
    int value;
    int index;
} MinLoc;

int main(int argc, char *argv[]) {
    int rank, size;
    int *array = NULL;
    int array_size, sub_array_size;
    MinLoc array_minloc, sub_array_minloc;
    array_minloc.value = INT_MAX;
    array_minloc.index = -1;
    sub_array_minloc.value = INT_MAX;
    sub_array_minloc.index = -1;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        scanf("%d", &array_size);
        srand(time(NULL));

        // Inicializar el arreglo con números consecutivos
        array = (int *)malloc(array_size * sizeof(int));
        for (int i = 0; i < array_size; i++) {
            array[i] = rand() % 100;
        }

        printf("Arreglo inicial: ");
        for (int i = 0; i < array_size; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");
    }

    // Se transmite el tamaño del arreglo a todos los procesos
    MPI_Bcast(&array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular el tamaño de cada subarreglo
    sub_array_size = array_size / size;

    // Cada proceso recibe su parte del arreglo
    int *sub_array = (int *)malloc(sub_array_size * sizeof(int));
    MPI_Scatter(array, sub_array_size, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada proceso calcula el menor elemento y su posición en su parte del arreglo
    for (int i = 0; i < sub_array_size; i++) {
        if (sub_array[i] < sub_array_minloc.value) {
            sub_array_minloc.value = sub_array[i];
            sub_array_minloc.index = i + (rank * sub_array_size);
        }
    }

    // Encontrar el menor elemento y su posición en todo el arreglo
    MPI_Reduce(&sub_array_minloc, &array_minloc, 1, MPI_2INT, MPI_MINLOC, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Menor elemento: %d, Posición: %d\n", array_minloc.value, array_minloc.index);
    }

    // Liberar memoria
    if (rank == 0) {
        free(array);
    }
    free(sub_array);

    MPI_Finalize();
    return 0;
}