#include <mpi.h>
#include <iostream>
#include <stdio.h>
#include <cmath>

#define PI 3.14159265

using namespace std;

double **allocate2DArray(int size);
void freeArray(double **matrix, int size);
void displayMatrix(int size, double **matrix);

int main(int argc, char *argv[])
{
    int rank, size;

    int matrSize = 10;
    int blockSize = matrSize * matrSize / 25;

    double globalBlocks[blockSize + 1][25];

    for (int i = 0; i < 25; i++)
    {
        globalBlocks[0][i] = 0;
    }

    double localVector[25];

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        double **A = allocate2DArray(matrSize);

        for (int i = 0; i < matrSize; i++)
        {
            for (int j = 0; j < matrSize; j++)
            {
                double x = i / 10.0;
                double y = j / 10.0;

                A[i][j] = sin(x * PI / 180.0) + exp(y);
            }
        }

        displayMatrix(matrSize, A);

        int count = 0;

        double **blocks = new double *[blockSize];
        for (int i = 0; i < blockSize; i++)
        {
            blocks[i] = new double[25];
        }

        for (int i = 0; i < blockSize; i++)
        {
            if ((i % 2 == 0) && (i == 0))
            {

                for (int j = 0; j < matrSize / 2; j++)
                {
                    for (int k = 0; k < matrSize / 2; k++)
                    {
                        blocks[i][j * (matrSize / 2) + k] = A[j][k];
                        globalBlocks[i + 1][j * (matrSize / 2) + k] = A[j][k];
                    }
                }
            }

            if ((i % 2 == 0) && (i == 2))
            {

                for (int j = matrSize / 2; j < matrSize; j++)
                {
                    for (int k = 0; k < matrSize / 2; k++)
                    {
                        blocks[i][(j - (matrSize / 2)) * (matrSize / 2) + k] = A[j][k];
                        globalBlocks[i + 1][(j - (matrSize / 2)) * (matrSize / 2) + k] = A[j][k];
                    }
                }
            }

            if ((i % 2 != 0) && (i == 1))
            {

                for (int j = 0; j < matrSize / 2; j++)
                {
                    for (int k = matrSize / 2; k < matrSize; k++)
                    {
                        blocks[i][j * (matrSize / 2) + (k - (matrSize / 2))] = A[j][k];
                        globalBlocks[i + 1][j * (matrSize / 2) + (k - (matrSize / 2))] = A[j][k];
                    }
                }
            }

            if ((i % 2 != 0) && (i == 3))
            {

                for (int j = matrSize / 2; j < matrSize; j++)
                {
                    for (int k = matrSize / 2; k < matrSize; k++)
                    {
                        blocks[i][(j - (matrSize / 2)) * (matrSize / 2) + (k - (matrSize / 2))] = A[j][k];
                        globalBlocks[i + 1][(j - (matrSize / 2)) * (matrSize / 2) + (k - (matrSize / 2))] = A[j][k];
                    }
                }
            }
        }

        cout << "Получившиеся блоки: " << endl;

        for (int l = 0; l < blockSize; l++)
        {
            cout << "Блок №" << (l + 1) << ": ";
            for (int m = 0; m < 25; m++)
            {
                if (m % 5 == 0)
                {
                    cout << endl;
                }

                cout << blocks[l][m] << "   ";
            }
            cout << endl;
        }

        freeArray(blocks, blockSize);

        freeArray(A, matrSize);
    }

    MPI_Scatter(globalBlocks, 25, MPI_DOUBLE, localVector, 25, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    cout << "Процесс №" << rank << " -> данные: " << endl;
    for (int i = 0; i < 25; i++)
    {
        if (i % 5 == 0)
        {
            cout << endl;
        }
        cout << localVector[i] << " ";
        localVector[i] = 69;
    }

    cout << endl
         << endl;

    MPI_Gather(localVector, 25, MPI_DOUBLE, globalBlocks, 25, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        cout << "Процесс №" << rank << " -> данные:" << endl;
        for (int i = 1; i < blockSize; i++)
        {
            for (int j = 0; j < 25; j++)
            {
                if (j % 10 == 0)
                {
                    cout << endl;
                }
                cout << globalBlocks[i][j] << "     ";
            }
        }
    }

    MPI_Finalize();

    return 0;
}

double **allocate2DArray(int size)
{
    double **a = new double *[size];
    for (int i = 0; i < size; i++)
    {
        a[i] = new double[size];
    }

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            a[i][j] = 0;
        }
    }

    return a;
}

void freeArray(double **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        delete[] matrix[i];
    }

    delete[] matrix;
}

void displayMatrix(int size, double **matrix)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            cout << matrix[i][j] << "       ";
        }
        cout << endl;
    }

    cout << endl;
}
