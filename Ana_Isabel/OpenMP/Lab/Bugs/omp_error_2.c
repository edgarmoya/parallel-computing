/******************************************************************************
* FILE: omp_error_2.c

******************************************************************************/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) 
{
int nthreads, i, tid;
float total=0.0;

/*** Spawn parallel region ***/
#pragma omp parallel private(tid) shared(nthreads) reduction(+:total)
  {
  /* Obtain thread number */
  tid = omp_get_thread_num();
  /* Only master thread does this */
  if (tid == 0) {
    nthreads = omp_get_num_threads();
    printf("Number of threads = %d\n", nthreads);
    }
  printf("Thread %d is starting...\n",tid);

  #pragma omp barrier

  /* do some work */
  //total = 0.0;
  #pragma omp for schedule(dynamic,10) private(i) //tambien se puede poner el reduction aqui
  for (i=0; i<1000000; i++) 
     total = total + i*1.0;

  printf ("Thread %d is done! Total= %e\n",tid,total);

  } /*** End of parallel region ***/
  printf("El total es: %e",total);
}
