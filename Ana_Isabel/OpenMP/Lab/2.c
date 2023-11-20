/******************************************************************************
Ana Isabel Portal Diaz

Ejercicio 2
Implemente un programa paralelo utilizando OpenMP que calcule la media de los 
elementos de un arreglo. Para inicializar el arreglo puede emplear la siguiente 
función y puede utilizar la versión serie del cálculo de la media

void randInit(int matrix[], const int n, const int bound) {
srand(time(NULL));
for (int i = 0; i < n; i++)
matrix[i] = rand() % bound;
}

double mean(int matrix[], const int n) {
double sum = 0;
for (int i = 0; i < n; i++)
sum += matrix[i];
return (double)sum / n;
}

******************************************************************************/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void randInit(int array[], const int n, const int bound) {
  srand(time(NULL));
  for (int i = 0; i < n; i++)
    array[i] = rand() % bound;
}

double mean(int array[], const int n) {
  double sum = 0;
  for (int i = 0; i < n; i++)
    sum += array[i];
  return (double)sum / n;
}

int main (int argc, char *argv[]) {
 
  int N, bound, nthreads; // tamagno del arreglo, valor maximo de los elementos, cantida de hilos
  
  if (argc != 4) {
    printf("Uso: %s <tamaño del arreglo> <cota de elementos> <cantidad de hilos>\n", argv[0]);
    return 0;
  }

  if ((N = atoi(argv[1])) == 0 || (bound = atoi(argv[2])) == 0) {
    printf("El tamaño del arreglo y la cota de elementos deben ser números enteros.\n");
    return 0;
  }
  
  nthreads = atoi(argv[3]); 

  int *matrix = malloc(sizeof(int)*N); 
  randInit(matrix, N, bound);

  omp_set_num_threads(nthreads);

  double parallel_sum = 0.0;
  double parallel_mean = 0.0;

  // Calcular tiempo de ejecución
  double inicial_time, final_time, exec_time; 
  inicial_time = omp_get_wtime();

  // Calculo de la media en paralelo
  #pragma omp parallel for reduction(+:parallel_sum)
  for (int i = 0; i < N; i++) {
    parallel_sum += matrix[i];
  }
  parallel_mean = parallel_sum / N;
  
  printf("Media calculada en paralelo es: %.2f\n", parallel_mean);

  // Resultados del tiempo de ejecución en paralelo
  final_time = omp_get_wtime(); 
  exec_time = final_time - inicial_time; 
  printf("Tiempo de ejecución en paralelo es: %.4f\n", exec_time); 

  // Calcula la media en serie para comparar
  double time_spent = 0.0;
  clock_t begin = clock();
  
  double serial_mean = mean(matrix, N);

  clock_t end = clock();
  printf("Media calculada en serie es: %.2f\n", serial_mean);

  time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

  printf("Tiempo de ejecución en serie: %.4f\n", time_spent);

  //Liberar memoria
  free(matrix);

  return 0;
}

