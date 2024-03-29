#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void){
    int i;
    int x = 100;
    omp_set_num_threads(4);
    #pragma omp parallel for private(x)
        for(i=0; i<=10; i++){
            x = i + 10;
            printf("Thread number: %d x: %d \n", omp_get_thread_num(), x);
        }
    printf("x is %d\n", x);
    return EXIT_SUCCESS;
}