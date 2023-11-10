#include <stdio.h>
#include <omp.h>

int main() {
    int N = 10;
    int i, j, prime, total = 0;
    omp_set_num_threads(10);

    #pragma omp parallel for private(j, prime) reduction(+:total)
    for (i = 2; i <= N; i++) {
        prime = 1;
        for (j = 2; j < i; j++) {
            if (i % j == 0) {
                prime = 0;
                break;
            }
        }
        total += prime;
    }

    printf("Cantidad de números primos entre 1 y %d: %d\n", N, total);

    return 0;
}
