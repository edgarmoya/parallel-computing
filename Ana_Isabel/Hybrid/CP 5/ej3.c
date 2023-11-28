/******************************************************************************
Ana Isabel Portal Díaz

Ejercicio 3

A partir del ejemplo del cálculo de pi que aparece en la conferencia de OpenMP:
A) Escriba un programa híbrido para dicho calculo.
B) Calcule el error cometido asumiendo el siguiente valor de PI
PI = 3.141592653589793238462643; para distintos valores de pasos
(num_steps)

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char *argv[]) {

    int rank, size, i;
    double step, pi, error, x;
    long num_steps, iter_per_process;
    double sum = 0.0, local_sum = 0.0;

    // Valor verdadero de PI
    const double real_pi = 3.141592653589793238462643; 

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {

        //Comprobar requerimientos 
        if (argc != 2) {
            fprintf(stderr, "Uso: %s <cantidad de pasos>\n", argv[0]);
            return 1;
        }

        // Obtener la cantidad de pasos
        num_steps = atol(argv[1]);       

        // Comprobar que la cantidad de pasos es múltiplo de la cantidad de procesos
        if (num_steps % size != 0) {
            printf("La cantidad de pasos debe ser múltiplo de la cantidad de procesos.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        iter_per_process = num_steps / size;
        step = 1.0/(double) num_steps;
    }

    // Difundir a todos los procesos
    MPI_Bcast(&iter_per_process, 1, MPI_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&step, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Calcular la contribución local de cada proceso 
    #pragma omp parallel for private(i, x) shared(rank, iter_per_process, step) reduction(+:local_sum)
    for (i = rank * iter_per_process; i < rank * iter_per_process + iter_per_process; i++) {
        x = (i + 0.5) * step;
        local_sum += 4.0 / (1.0 + x * x);
    }

    // Realizar una suma global a traves de un reduce
    MPI_Reduce(&local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Calcular pi utilizando la suma global 
    if (rank == 0) {
        pi = step * sum;
        // Mostrar el resultado
        printf("El valor calculado de PI es: %.20f\n", pi);

        // Calcular y mostrar el error cometido
        error = fabs(pi - real_pi);
        printf("El error cometido es: %.20f\n", error);
    }

    MPI_Finalize();
    return 0;
}
