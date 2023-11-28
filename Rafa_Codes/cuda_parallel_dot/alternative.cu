/*
    Alternativa del CUDA by example cap 5
*/
#include <bits/stdc++.h>
#define imin(a, b) (a < b ? a : b)

const int N = 10000 * 1024;
const int threadsPerBlock = 64;
const int blocksPerGrid = imin(32, (N + threadsPerBlock - 1) / threadsPerBlock);

__global__ void dot(int *a, int *b, int *c)
{
    __shared__ int cache[threadsPerBlock];

    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    int cacheIndex = threadIdx.x;

    long long temp = 0;
    while (tid < N)
    {
        temp += a[tid] * b[tid];
        tid += blockDim.x * gridDim.x;
    }

    // set the cache values
    cache[cacheIndex] = temp;

    // synchronize threads in this block
    __syncthreads();

    // for reductions, threadsPerBlock must be a power of 2
    // because of the following code
    long long i = blockDim.x / 2;
    while (i != 0)
    {
        if (cacheIndex < i)
            cache[cacheIndex] += cache[cacheIndex + i];
        __syncthreads();
        i /= 2;
    }
    if (cacheIndex == 0)
        c[blockIdx.x] = cache[0];
}

long long main(void)
{
    
    long long *a, *b, c, *partial_c;
    long long *dev_a, *dev_b, *dev_partial_c;
    
    // allocate memory on the CPU side
    a = (long long *)malloc(N * sizeof(long long));
    b = (long long *)malloc(N * sizeof(long long));
    partial_c = (long long *)malloc(blocksPerGrid * sizeof(long long));
    
    // allocate the memory on the GPU
    cudaMalloc((void **)&dev_a, N * sizeof(long long));
    cudaMalloc((void **)&dev_b, N * sizeof(long long));
    cudaMalloc((void **)&dev_partial_c, blocksPerGrid * sizeof(long long));
    
    // fill in the host memory with data
    for (long long i = 0; i < N; i++)
    {
        a[i] = 1;
        b[i] = 1;
    }
    
    // copy the arrays ‘a’ and ‘b’ to the GPU
    cudaMemcpy(dev_a, a, N * sizeof(long long), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_b, b, N * sizeof(long long), cudaMemcpyHostToDevice);

    auto start = std::chrono::steady_clock::now();

    dot<<<blocksPerGrid, threadsPerBlock>>>(dev_a, dev_b, dev_partial_c);

    // copy the array 'c' back from the GPU to the CPU
    cudaMemcpy(partial_c, dev_partial_c, blocksPerGrid * sizeof(int), cudaMemcpyDeviceToHost);

    // finish up on the CPU side
    c = 0;
    for (int i = 0; i < blocksPerGrid; i++)
    {
        c += partial_c[i];
    }

    auto end = std::chrono::steady_clock::now();
    auto elapsedSequential = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    printf("Tiempo de ejecución paralelo: %ld ms\n", elapsedSequential.count());

    printf( "GPU value %d = %d?\n", c, 2 * sum_squares( (int)(N - 1) ) );


    auto start_s = std::chrono::steady_clock::now();
    int cpu_res = 0;
    for (int i = 0; i < N; ++i)
    {
        cpu_res += a[i] * b[i];
    }
    printf("cpu result = %d\n", cpu_res);
    auto end_s = std::chrono::steady_clock::now();
    auto elapsedSequential_s = std::chrono::duration_cast<std::chrono::milliseconds>(end_s - start_s);
    printf("Tiempo de ejecución secuencial: %ld ms\n", elapsedSequential_s.count());

    // free memory on the GPU side
    cudaFree(dev_a);
    cudaFree(dev_b);
    cudaFree(dev_partial_c);
    
    // free memory on the CPU side
    free(a);
    free(b);
    free(partial_c);
}