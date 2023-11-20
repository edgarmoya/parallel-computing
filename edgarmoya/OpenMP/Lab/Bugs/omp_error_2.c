
/* Edgar Moya Cáceres */

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) 
{
  int nthreads, i, tid;
  float total;

  // Se añaden cláusulas de alcance de los datos para "total", "i" y "tid"
  #pragma omp parallel shared(total) private(i, tid)  
  {
    tid = omp_get_thread_num();

    if (tid == 0) {
      nthreads = omp_get_num_threads();
      printf("Number of threads = %d\n", nthreads);
    }
    printf("Thread %d is starting...\n", tid);

    #pragma omp barrier

    /* do some work */
    #pragma omp for schedule(dynamic, 10)
    for (i = 0; i < 10000; i++){
      #pragma omp critical
      total = total + i*1.0;
    }
    
    printf ("Thread %d is done! Total= %e\n", tid, total);
  }
}
