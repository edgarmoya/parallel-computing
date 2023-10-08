/* Adicionando la posibilidad de que cada proceso tenga la suma total */

/* Edgar Moya Cáceres */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void print_matrix(int *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    int rank, size;
    int rows, cols, rows_x_process, remainder_rows;
    MPI_Datatype dtype_rows;
    int *matrix = NULL;
    int *send_counts = NULL;
    int *displacements = NULL;
    int *local_matrix = NULL;
    int *row_sum = NULL;
    int *global_row_sum = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Ingrese el número de filas: \n");
        scanf("%d", &rows);
        printf("Ingrese el número de columnas: \n");
        scanf("%d", &cols);

        // Asignación de memoria para la matriz
        matrix = (int *)malloc(rows * cols * sizeof(int));

        // Inicializar la matriz con valores aleatorios
        srand(time(NULL));
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                matrix[i * cols + j] = rand() % 10;
            }
        }

        printf("Matriz inicial:\n");
        print_matrix(matrix, rows, cols);
    }

    // Difundir el tamaño de la matriz a todos los procesos
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Type_vector(1, cols, cols, MPI_INT, &dtype_rows);
    MPI_Type_commit(&dtype_rows);

    // Calcular la cantidad de filas por proceso y las filas restantes
    rows_x_process = rows / size;
    remainder_rows = rows % size;

    // Calcular la cantidad de filas a asignadas por cada procesos 
    send_counts = (int *)malloc(size * sizeof(int));
    displacements = (int *)malloc(size * sizeof(int));
    int current_displacement = 0;

    for (int i = 0; i < size; i++) {
        send_counts[i] = (i < remainder_rows) ? (rows_x_process + 1) : rows_x_process;
        displacements[i] = current_displacement;
        current_displacement += send_counts[i];
    }

    // Distribuir las filas de la matriz a todos los procesos
    local_matrix = (int *)malloc(send_counts[rank] * cols * sizeof(int));
    MPI_Scatterv(matrix, send_counts, displacements, dtype_rows, local_matrix, send_counts[rank], dtype_rows, 0, MPI_COMM_WORLD);

    // Calcular la suma de cada fila localmente
    row_sum = (int *)malloc(send_counts[rank] * sizeof(int));
    int total_row_sum;
    for (int i = 0; i < send_counts[rank]; i++) {
        row_sum[i] = 0;
        for (int j = 0; j < cols; j++) {
            row_sum[i] += local_matrix[i * cols + j];
            total_row_sum += local_matrix[i * cols + j];
        }
    }

    // Recolectar las sumas de las filas de todos los procesos
    if (rank == 0) {
        global_row_sum = (int *)malloc(rows * sizeof(int));
    }
    MPI_Gatherv(row_sum, send_counts[rank], MPI_INT, global_row_sum, send_counts, displacements, MPI_INT, 0, MPI_COMM_WORLD);

    // Proceso 0 imprime el vector resultante con las sumas de cada fila
    if (rank == 0) {
        printf("\nSuma de los elementos de cada fila:\n");
        for (int i = 0; i < rows; i++) {
            printf("Fila %d -> %d\n", i+1, global_row_sum[i]);
        }
    }

    // Sumar filas y difundir a cada proceso el resultado
    int total_sum;
    MPI_Allreduce(&total_row_sum, &total_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    
    // Mostrar la suma total acumulada en cada proceso
    printf("El proceso %d conoce que la suma total es: %d\n", rank, total_sum);

    // Liberar la memoria
    free(row_sum);
    free(local_matrix);
    free(send_counts);
    free(displacements);

    if (rank == 0) {
        free(matrix);
        free(global_row_sum);
    }

    MPI_Finalize();
    return 0;
}