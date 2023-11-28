#include "kernel.h"
#include <stdio.h>
#define TPB 64
#define ATOMIC 1 // 0 for non-atomic addition

__global__
void dotKernel(int *d_res, const int *d_a, const int *d_b, int n) {
    const int idx = threadIdx.x + blockDim.x * blockIdx.x;
    if (idx >= n) return;

    /*int prod = d_a[idx] * d_b[idx];
    if (ATOMIC) {
        atomicAdd(d_res, prod);
    } else {
        *d_res += prod;
    }*/

    const int s_idx = threadIdx.x;
    __shared__ int s_prod[TPB];
    s_prod[s_idx] = d_a[idx] * d_b[idx];
    __syncthreads();
    if (s_idx == 0) {
        int blockSum = 0;
        for (int j = 0; j < blockDim.x; ++j) {
            blockSum += s_prod[j];
        }

        if (ATOMIC) {
            atomicAdd(d_res, blockSum);
        } else {
            *d_res += blockSum;
        }
    }
}

void dotLauncher(int *res, const int *a, const int *b, int n) {
    int *d_res;
    int *d_a = 0;
    int *d_b = 0;

    cudaMalloc(&d_res, sizeof(int));
    cudaMalloc(&d_a, n*sizeof(int));
    cudaMalloc(&d_b, n*sizeof(int));

    cudaMemset(d_res, 0, sizeof(int));
    cudaMemcpy(d_a, a, n*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b, n*sizeof(int), cudaMemcpyHostToDevice);

    cudaEvent_t start_gpu, end_gpu;
    cudaEventCreate(&start_gpu);
    cudaEventCreate(&end_gpu);

    // Capturar el tiempo de inicio en la GPU
    cudaEventRecord(start_gpu);

    dotKernel<<<(n + TPB - 1)/TPB, TPB>>>(d_res, d_a, d_b, n);

    // Capturar el tiempo de finalización en la GPU
    cudaEventRecord(end_gpu);
    cudaEventSynchronize(end_gpu);

    // Calcular la duración e imprimir el resultado en milisegundos
    float duration_gpu;
    cudaEventElapsedTime(&duration_gpu, start_gpu, end_gpu);
    std::cout << "Tiempo de ejecución en GPU: " << duration_gpu << " ms" << std::endl;

    cudaMemcpy(res, d_res, sizeof(int), cudaMemcpyDeviceToHost);
    
    cudaFree(d_res);
    cudaFree(d_a);
    cudaFree(d_b);

    // Liberar eventos
    cudaEventDestroy(start_gpu);
    cudaEventDestroy(end_gpu);
}
