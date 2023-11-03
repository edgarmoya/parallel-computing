/* Implemente un programa paralelo que a partir de un valor aleatorio de x calcule:
• y = x2 en el proceso 0
• y = x3 en el proceso 1
• y = x4 en el proceso 2
debe imprimir el identificador del proceso y los valores de las variables en cada uno
a) Modifique el programa para que el valor de x sea el mismo en todos los procesos */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int random_val(int min, int max){
    return min + rand() % (max - min + 1);
}

int main(int argc, char *argv[]) {
    int rank, size;
    int x, y;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 3){
        printf("Esta aplicación debe ejecutarse con 3 procesos MPI.\n");
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        // Generar número aleatorio
        srand(time(NULL));
        x = random_val(0, 50);
    }
    
    // Se transmite a todos los procesos
    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular "y" en cada proceso
    if (rank == 0) {
        y = x * x;
    } else if (rank == 1) {
        y = x * x * x;
    } else if (rank == 2) {
        y = x * x * x * x;
    }

    printf("Proceso %d: x = %d, y = %d\n", rank, x, y);

    MPI_Finalize();
    return 0;
}
