#include <stdio.h>
#include <omp.h>

int main(int argc, char **argv){
    int i, thread_id, nloops, sum;
    omp_set_num_threads(3);

    #pragma omp parallel private(thread_id, nloops, sum)
        nloops = 0;
        sum = 0;
        #pragma omp for 
            for (i=0; i<10; ++i) {
                thread_id = omp_get_thread_num();
                printf("thread %d have i=%d \n", thread_id, i); 
                sum += i;
                ++nloops;
            }
        thread_id = omp_get_thread_num();
        printf("Thread %d performed %d iterations of the loop and their sum is %d.\n", thread_id, nloops, sum);
    
    return 0;
}
