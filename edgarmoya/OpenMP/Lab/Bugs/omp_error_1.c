/******************************************************************************
* FILE: omp_error_1.c
******************************************************************************/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N 50
#define CHUNKSIZE 5

int main (int argc, char *argv[]) {

  int i, chunk, tid;
  float a[N], b[N], c[N];

  /* Some initializations */
  for (i=0; i < N; i++)
    a[i] = b[i] = i * 1.0;
  chunk = CHUNKSIZE;

  #pragma omp parallel for shared(a,b,c,chunk) private(i,tid) schedule(static, chunk)
    // tid = omp_get_thread_num();
    // la directiva #pragma omp parallel for no está encerrando todo el bucle for
    for (i=0; i < N; i++) {
      tid = omp_get_thread_num(); // Se hace innecesariamente en cada iteración
      c[i] = a[i] + b[i];
      printf("tid= %d i= %d c[i]= %f\n", tid, i, c[i]);
    }
    /* end of parallel for construct */

  return 0;

}

