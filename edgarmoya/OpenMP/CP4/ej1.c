#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define N 20

int main(int argc, char **argv) {
    int tid, nthr; 
    // identificador del thread y numero de threads 
    int i, A[N];
    printf("\nIntroduce el numero de threads ---> ");
    scanf("%d", &nthr);

    #ifdef _OPENMP
        omp_set_num_threads(nthr);
    #endif

    #pragma omp parallel for schedule(static,2) private(i,tid)
        for (i=0; i<N; i++){
            tid = omp_get_thread_num ();
            printf ("Thread %d of %d is running\n", tid, nthr); 
            A[i] = tid;
            printf ("Thread %d has finished\n", tid);
        }

    for (i=0; i<N; i++)
        printf ("A(%d) = %d \n", i, A[i]);
    
    return (0);
}