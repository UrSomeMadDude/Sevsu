#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <time.h>
#include <chrono>
#include "mpi.h"
#include <vector>
#include <cstdlib>
#include <fstream>
#include "string"
#include <stdio.h>

using namespace std;

#define BLOCK_SIZE 2048
#define SIZE 8192

int *custom_sort(int *mas_min, int *mas_max, int spl);

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Status status;

    MPI_Request request = MPI_REQUEST_NULL;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Comm nthCube;
    const int nDim = 2;
    int processPerDim[nDim] = {2, 2};
    int period[nDim] = {1, 1};

    MPI_Cart_create(MPI_COMM_WORLD, nDim, processPerDim, period, true, &nthCube);

    int *array = new int[SIZE];
    for (int i = 0; i < SIZE; ++i)
    {
        array[i] = 999 - i;
    }

    int *mas = new int[BLOCK_SIZE];

    int rankInDim;
    MPI_Comm_rank(nthCube, &rankInDim);

    int rank_source, rank_desta, rank_destb;
    MPI_Cart_shift(nthCube, 0, 1, &rank_source, &rank_desta);
    MPI_Cart_shift(nthCube, 1, 1, &rank_source, &rank_destb);

    int block[BLOCK_SIZE];
    int recvBlock[BLOCK_SIZE];
    int c = 0;

    auto start = chrono::steady_clock::now();

    while (c < BLOCK_SIZE)
    {
        block[c] = array[rankInDim * BLOCK_SIZE + c];
        ++c;
    }

    if (rankInDim < size / 2)
    {
        MPI_Isend(&block, BLOCK_SIZE, MPI_INT, rankInDim + nDim, 99, MPI_COMM_WORLD, &request);

        MPI_Recv(&recvBlock, BLOCK_SIZE, MPI_INT, rankInDim + nDim, 99, MPI_COMM_WORLD, &status);

        for (int i = 0; i < BLOCK_SIZE; ++i)
        {
            mas[i] = 0;
        }

        mas = custom_sort(block, recvBlock, 0);
    }
    else
    {
        MPI_Isend(&block, BLOCK_SIZE, MPI_INT, rankInDim - nDim, 99, MPI_COMM_WORLD, &request);

        MPI_Recv(&recvBlock, BLOCK_SIZE, MPI_INT, rankInDim - nDim, 99, MPI_COMM_WORLD, &status);

        for (int i = 0; i < BLOCK_SIZE; ++i)
        {
            mas[i] = 0;
        }

        mas = custom_sort(block, recvBlock, 1);
    }

    for (int i = 0; i < BLOCK_SIZE; ++i)
    {
        block[i] = mas[i];
    }

    if (rankInDim % 2 == 0)
    {
        MPI_Isend(&block, BLOCK_SIZE, MPI_INT, rankInDim + (nDim / 2), 99, MPI_COMM_WORLD, &request);

        MPI_Recv(&recvBlock, BLOCK_SIZE, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);

        mas = custom_sort(block, recvBlock, 0);
    }
    else
    {
        MPI_Isend(&block, BLOCK_SIZE, MPI_INT, rankInDim - (nDim / 2), 99, MPI_COMM_WORLD, &request);

        MPI_Recv(&recvBlock, BLOCK_SIZE, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);

        mas = custom_sort(block, recvBlock, 1);
    }

    auto end = chrono::steady_clock::now();

    if (rankInDim == 0)
    {
        std::cout << "Time parralel in ns: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << "\n";
    }

    if (rankInDim == 1)
    {
        int n = SIZE;
        int d = n / 2;

        start = chrono::steady_clock::now();

        while (d > 0)
        {
            for (int i = 0; i < n - d; i++)
            {
                for (int j = i; j >= 0 && array[j] > array[j + d]; j--)
                {
                    int count = array[j];
                    array[j] = array[j + d];
                    array[j + d] = count;
                }
            }
            d = d / 2;
        }

        end = chrono::steady_clock::now();

        std::cout << "\nTime sequential in ns: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << "\n";
    }

    // cout << "Process " << rankInDim << endl;
    // for (int i = 0; i < BLOCK_SIZE; ++i)
    // {
    //     cout << rankInDim << "_" << mas[i] << "   ";
    // }
    // cout << endl;

    MPI_Finalize();

    return 0;
}

int *custom_sort(int *mas_min, int *mas_max, int spl)
{
    int S_mas[BLOCK_SIZE * 2];
    int *result = new int[BLOCK_SIZE];

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        S_mas[i] = mas_min[i];
    }
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        S_mas[BLOCK_SIZE + i] = mas_max[i];
    }

    int swap = 0;
    int n = BLOCK_SIZE * 2;
    int d = n / 2;

    while (d > 0)
    {
        for (int i = 0; i < n - d; i++)
        {
            for (int j = i; j >= 0 && S_mas[j] > S_mas[j + d]; j--)
            {
                swap = S_mas[j];
                S_mas[j] = S_mas[j + d];
                S_mas[j + d] = swap;
            }
        }
        d = d / 2;
    }

    if (spl)
    {
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            result[i] = S_mas[BLOCK_SIZE + i];
        }
        return result;
    }
    else
    {
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            result[i] = S_mas[i];
        }
        return result;
    }
}