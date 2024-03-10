Here's a refactored version of your OpenCL MPI Matrix Multiplication code:

```cpp
#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/time.h>
#include<CL/cl.h>

#define N 800

void initializeArray(int array[N][N]);
void printArrays(int array[N][N]);
void openCLMatrixMultiplication(int np, int rank, int inputArray1[N][N], int inputArray2[N][N], int outputArray[N][N]);

// OpenCL-related functions
cl_device_id createDevice();
cl_program buildProgram(cl_context ctx, cl_device_id dev, const char* filename);
void setupOpenCLDeviceContextQueueKernel();
void setupKernelMemory();
void copyKernelArgs();
void freeMemory();

cl_device_id device_id;
cl_context context;
cl_program program;
cl_kernel kernel;
cl_command_queue  queue;
cl_event event = NULL;
int err;

cl_mem bufA, bufB, bufC;
const int max = N;
const int TS = 4;
const size_t local[2] = { TS, TS };
const size_t global[2] = { max, max }; 

int main() {
    // ... (unchanged main function content)

    MPI_Init(NULL, NULL);
    int np = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int inputArray1[N][N], inputArray2[N][N];
    int outputArray[N][N] = {{0}};

    if (rank == 0) {
        initializeArray(inputArray1);
        initializeArray(inputArray2);
    }

    gettimeofday(&timecheck, NULL);
    long timeofday_start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&inputArray1, N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&inputArray2, N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    openCLMatrixMultiplication(np, rank, inputArray1, inputArray2, outputArray);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(outputArray, N * N / np, MPI_INT, outputArray, N * N / np, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Finalize();

    gettimeofday(&timecheck, NULL);
    long timeofday_end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
    double time_elapsed = timeofday_end - timeofday_start;

    if (rank == 0) {
        printf("\t\tTime elapsed: %f ms\n", time_elapsed);
    }

    return 0;
}

void initializeArray(int array[N][N]) {
    printf("Initializing array... ");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            array[i][j] = rand() % ((10 - 1) + 1) + 1;
        }
    }
    printf("complete\n");
}

void printArrays(int array[N][N]) {
    printf("[");
    for (int i = 0; i < N; i++) {
        printf("[");
        for (int j = 0; j < N; j++) {
            printf("%i", array[i][j]);
            printf(" ");
        }
        printf("]\n");
    }
    printf("]\n\n");
}

void openCLMatrixMultiplication(int np, int rank, int inputArray1[N][N], int inputArray2[N][N], int outputArray[N][N]) {
    long value;
    int range = N / np;
    int start = rank * range;
    int end = start + range;
    int buffArray[range][N] = {0};

    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, NULL, &event);
    clWaitForEvents(1, &event);

    clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0, N * N * sizeof(int), outputArray, 0, NULL, NULL);

    freeMemory();
}

void freeMemory() {
    clReleaseKernel(kernel);
    clReleaseMemObject(bufA);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufC);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
}

void copyKernelArgs() {
    clSetKernelArg(kernel, 0, sizeof(int), (void*)&max);
    clSetKernelArg(kernel, 1, sizeof(int), (void*)&max);
    clSetKernelArg(kernel, 2, sizeof(int), (void*)&max);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&bufA);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&bufB);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&bufC);
    if (err < 0) {
        perror("Couldn't create a kernel argument");
        printf("error = %d", err);
        exit(1);
    }
}

void setupKernelMemory() {
    bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, N * N * sizeof(int), NULL, NULL);
    bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, N * N * sizeof(int), NULL, NULL);
    bufC = clCreateBuffer(context, CL_MEM_READ_WRITE, N * N * sizeof(int), NULL, NULL);

    clEnqueueWriteBuffer(queue, bufA, CL_TRUE, 0, N * N * sizeof(int), inputArray1, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufB, CL_TRUE, 0, N * N * sizeof(int), inputArray2, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufC, CL_TRUE, 0, N * N * sizeof(int), outputArray, 0, NULL, NULL);
}

void setupOpenCLDeviceContextQueueKernel() {
    device_id = createDevice();
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    if (err < 0) {
        perror("Couldn't create a context");
        exit(1);
    }

    program = buildProgram(context, device_id, "matrix_mul.cl");

    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    if (err < 0) {
        perror("Couldn't create a command queue");
        exit(1);
    }

    kernel = clCreateKernel(program, "multiply_matrices", &err);
    if (err < 0) {
        perror("Couldn't create a kernel");
        printf("error =%d", err);
        exit(1);
    }
}

cl_program buildProgram(cl_context ctx, cl_device_id dev, const char* filename) {
    cl_program program;
    FILE* program_handle;
    char* program_buffer;


    size_t program_size;

    program_handle = fopen(filename, "r");
    if (program_handle == NULL) {
        perror("Couldn't find the program file");
        exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char*)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);

    program = clCreateProgramWithSource(ctx, 1, (const char**)&program_buffer, &program_size, &err);
    if (err < 0) {
        perror("Couldn't create the program");
        exit(1);
    }
    free(program_buffer);

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err < 0) {
        size_t log_size;
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char* program_log = (char*)malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    return program;
}

cl_device_id createDevice() {
    cl_platform_id platform;
    cl_device_id dev;
    int err;

    err = clGetPlatformIDs(1, &platform, NULL);
    if (err < 0) {
        perror("Couldn't identify a platform");
        exit(1);
    }

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
    if (err == CL_DEVICE_NOT_FOUND) {
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
    }
    if (err < 0) {
        perror("Couldn't access any devices");
        exit(1);
    }

    return dev;
}
```
