/* Escriba un programa donde se tiene un vector de tamaño N y P procesos. (N es divisible
por la cantidad de procesos). Este vector es inicializado por el proceso 0 y enviado 
en partes iguales a cada uno de los procesos. Cada proceso realiza la suma de su porción
del vector y la retorna al proceso 0 donde se realiza e imprime la suma total de los
elementos del vector. Debe auxiliarse de las funciones de comunicación colectivas. */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int *array = NULL;
    int array_size, sub_array_size;
    int total_sum, sub_array_sum = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        scanf("%d", &array_size);

        // Inicializar el arreglo con números consecutivos
        array = (int *)malloc(array_size * sizeof(int));
        for (int i = 0; i < array_size; i++) {
            array[i] = i + 1;
        }
    }

    // Se transmite el tamaño del arreglo a todos los procesos
    MPI_Bcast(&array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular el tamaño de cada subarreglo
    sub_array_size = array_size / size;

    // Cada proceso recibe su parte del arreglo
    int *sub_array = (int *)malloc(sub_array_size * sizeof(int));
    MPI_Scatter(array, sub_array_size, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada proceso calcula la suma de su parte del arreglo
    for (int i = 0; i < sub_array_size; i++) {
        sub_array_sum += sub_array[i];
    }

    printf("Suma parcial de %d: %d\n", rank, sub_array_sum);

    // Cada proceso envía su suma parcial al proceso 0
    MPI_Reduce(&sub_array_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Suma total: %d\n", total_sum);
    }

    // Liberar memoria
    if (rank == 0) {
        free(array);
    }
    free(sub_array);

    MPI_Finalize();
    return 0;
}
