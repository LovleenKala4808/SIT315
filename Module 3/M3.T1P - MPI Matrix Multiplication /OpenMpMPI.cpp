#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define N 800

void initializeArray(int array[N][N]);
void printArrays(int array[N][N]);
void matrixMultiplication(int np, int rank, int inputArray1[N][N], int inputArray2[N][N], int outputArray[N][N]);
void printOutput(int outputArray[N]);

struct timeval timecheck;

int main() {
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

    if (rank == 0) {
        printf("MPI Matrix Multiplication.\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&inputArray1, N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&inputArray2, N * N, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    matrixMultiplication(np, rank, inputArray1, inputArray2, outputArray);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();

    if (rank == 0) {
        printf("\nResultant Matrix:\n");
        printArrays(outputArray);
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
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", array[i][j]);
        }
        printf("\n");
    }
}

void matrixMultiplication(int np, int rank, int inputArray1[N][N], int inputArray2[N][N], int outputArray[N][N]) {
    int range = N / np;
    int start = rank * range;
    int end = start + range;

    for (int i = start; i < end; i++) {
        for (int j = 0; j < N; j++) {
            int value = 0;
            for (int k = 0; k < N; k++) {
                value += inputArray1[i][k] * inputArray2[k][j];
            }
            outputArray[i][j] = value;
        }
    }
}
