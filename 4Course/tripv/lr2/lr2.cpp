#include <mpi.h>
#include <iostream>
#include <stdio.h>
#include <cmath>

#define PI 3.14159265

using namespace std;

double **allocate2DArray(int size);
void freeArray(double **matrix, int size);
void displayMatrix(int size, double **matrix);
double getMin(int size, double *vector);
double getMax(int size, double *vector);

int main(int argc, char *argv[])
{
    int rank, size;

    const int ROOT = 0;

    int matrSize = 10;
    int blockSize = matrSize * matrSize / 25;

    double globalBlocks[5][25];
    double globalMin;
    double globalMax;

    double localVector[25];
    double localMin;
    double localMax;

    MPI_Status status;

    int errCode;

    if ((errCode = MPI_Init(&argc, &argv)) != 0)
    {
        return errCode;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int min_ranks[5];
    int max_ranks[5];
    int min_c = 0, max_c = 0;

    MPI_Comm min_comm, max_comm;
    MPI_Group group_world, min_group, max_group;

    MPI_Comm_group(MPI_COMM_WORLD, &group_world);

    for (int i = 0; i < size; i++)
    {
        if (i == ROOT)
        {
            min_ranks[min_c] = i;
            max_ranks[max_c] = i;
            min_c++;
            max_c++;
        }

        if ((i % 2 != 0) && (i != ROOT))
        {
            min_ranks[min_c] = i;
            min_c++;
        }

        if ((i % 2 == 0) && (i != ROOT))
        {
            max_ranks[max_c] = i;
            max_c++;
        }
    }

    MPI_Group_incl(group_world, 5, min_ranks, &min_group);
    MPI_Group_incl(group_world, 5, max_ranks, &max_group);

    MPI_Comm_create(MPI_COMM_WORLD, min_group, &min_comm);
    MPI_Comm_create(MPI_COMM_WORLD, max_group, &max_comm);

    int subrank, subsize;

    if (rank == ROOT)
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

        for (int i = 0; i < blockSize; i++)
        {
            if ((i % 2 == 0) && (i == 0))
            {

                for (int j = 0; j < matrSize / 2; j++)
                {
                    for (int k = 0; k < matrSize / 2; k++)
                    {
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
                        globalBlocks[i + 1][(j - (matrSize / 2)) * (matrSize / 2) + (k - (matrSize / 2))] = A[j][k];
                    }
                }
            }
        }

        for (int i = 0; i < 25; ++i)
        {
            globalBlocks[0][i] = globalBlocks[1][i];
        }

        cout << "Result blocks: " << endl;
        for (int l = 0; l < blockSize; l++)
        {
            cout << "Block #" << (l + 1) << ": ";
            for (int m = 0; m < 25; m++)
            {
                if (m % 5 == 0)
                {
                    cout << endl;
                }

                cout << globalBlocks[l][m] << "   ";
            }
            cout << endl;
        }

        globalMax = globalBlocks[0][0];
        globalMin = globalBlocks[0][0];
        localMax = globalBlocks[0][0];
        localMin = globalBlocks[0][0];

        freeArray(A, matrSize);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (min_comm != MPI_COMM_NULL)
    {
        cout << "Counting local minimumes..." << endl;

        MPI_Comm_rank(min_comm, &subrank);
        MPI_Comm_size(min_comm, &subsize);

        MPI_Scatter(globalBlocks, 25, MPI_DOUBLE, localVector, 25, MPI_DOUBLE, ROOT, min_comm);

        localMin = getMin(25, localVector);

        cout << "Rank #" << subrank << " local min: " << localMin << endl;

        MPI_Reduce(&localMin, &globalMin, 1, MPI_DOUBLE, MPI_MIN, ROOT, min_comm);
    }

    if (max_comm != MPI_COMM_NULL)
    {
        cout << "Counting local maximumes..." << endl;

        MPI_Comm_rank(max_comm, &subrank);
        MPI_Comm_size(max_comm, &subsize);

        MPI_Scatter(globalBlocks, 25, MPI_DOUBLE, localVector, 25, MPI_DOUBLE, ROOT, max_comm);

        localMax = getMax(25, localVector);

        cout << "Rank #" << subrank << " local max: " << localMax << endl;

        MPI_Reduce(&localMax, &globalMax, 1, MPI_DOUBLE, MPI_MAX, ROOT, max_comm);
    }

    if (rank == ROOT)
    {
        cout << endl
             << "Global min: " << globalMin << endl;
        cout << endl
             << "Global max: " << globalMax << endl;
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

double getMin(int size, double *vector)
{
    double min = vector[0];

    for (int i = 1; i < size; ++i)
    {
        if (vector[i] < min)
        {
            min = vector[i];
        }
    }

    return min;
}

double getMax(int size, double *vector)
{
    double max = vector[0];

    for (int i = 1; i < size; ++i)
    {
        if (vector[i] > max)
        {
            max = vector[i];
        }
    }

    return max;
}