#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>

#define N 900

void initializeArray(std::vector<std::vector<int>>& array);
void printArrays(const std::vector<std::vector<int>>& array);
void matrixMultiplication(int np, int rank, const std::vector<std::vector<int>>& inputArray1,
                           const std::vector<std::vector<int>>& inputArray2, std::vector<std::vector<int>>& outputArray);
void printOutput(const std::vector<std::vector<int>>& outputArray);

struct timeval timecheck;

int main() {
    MPI_Init(NULL, NULL);

    int np = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<std::vector<int>> inputArray1(N, std::vector<int>(N));
    std::vector<std::vector<int>> inputArray2(N, std::vector<int>(N));
    std::vector<std::vector<int>> outputArray(N, std::vector<int>(N, 0));

    if (rank == 0) {
        initializeArray(inputArray1);
        initializeArray(inputArray2);
    }

    if (rank == 0) {
        std::cout << "MPI Matrix Multiplication." << std::endl;
    }

    gettimeofday(&timecheck, NULL);
    long timeofday_start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(inputArray1.data(), N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(inputArray2.data(), N * N, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    matrixMultiplication(np, rank, inputArray1, inputArray2, outputArray);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();

    gettimeofday(&timecheck, NULL);
    long timeofday_end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
    double time_elapsed = timeofday_end - timeofday_start;

    if (rank == 0) {
        std::cout << "\t\tTime elapsed: " << time_elapsed << " ms" << std::endl;
    }

    return 0;
}

void initializeArray(std::vector<std::vector<int>>& array) {
    std::cout << "Initializing array... ";
    std::srand(std::time(0));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            array[i][j] = std::rand() % 10 + 1;
        }
    }
    std::cout << "complete" << std::endl;
}

void printArrays(const std::vector<std::vector<int>>& array) {
    std::cout << "[";
    for (const auto& row : array) {
        std::cout << "[";
        for (int element : row) {
            std::cout << element << " ";
        }
        std::cout << "]\n";
    }
    std::cout << "]\n\n";
}

void matrixMultiplication(int np, int rank, const std::vector<std::vector<int>>& inputArray1,
                           const std::vector<std::vector<int>>& inputArray2, std::vector<std::vector<int>>& outputArray) {
    long value;
    int range = N / np;
    int start = rank * range;
    int end = start + range;
    std::vector<std::vector<int>> buffArray(range, std::vector<int>(N, 0));

    for (int i = start; i < end; ++i) {
        for (int j = 0; j < N; ++j) {
            value = 0;
            for (int k = 0; k < N; ++k) {
                value += inputArray1[i][k] * inputArray2[k][j];
            }
            buffArray[i - start][j] = value;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(buffArray.data(), range * N, MPI_INT, outputArray.data(), range * N, MPI_INT, 0, MPI_COMM_WORLD);
}
