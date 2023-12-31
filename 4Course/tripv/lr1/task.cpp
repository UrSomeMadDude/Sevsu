#include <mpi.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <cmath>
#include <fstream>

using namespace std;

int **readMatrixFromFile(string filename, int size);
int *toVector(int **matrix, int size);
int **allocate2DArray(int size);
void freeArray(int **matrix, int size);
void displayMatrix(int size, int **matrix);
int countVectorDet(int *v);
int **formTriMatrix(int row, int col, int **matr);

int defaultMatrixSize = 4;
int minorMatrixSize = 3;

int main(int argc, char *argv[])
{
    int rank, size;

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {

        bool finishedProcesses[4] = {false,
                                     false,
                                     false,
                                     false};

        int **triangleMatrix = allocate2DArray(3);

        int determinant = 0;

        int **matrix = readMatrixFromFile("data.txt", defaultMatrixSize);
        cout << endl
             << "Initial matrix: " << endl
             << endl;
        displayMatrix(defaultMatrixSize, matrix);

        for (int i = 0; i < defaultMatrixSize; i++)
        {
            triangleMatrix = formTriMatrix(0, i, matrix);
            int *vector = toVector(triangleMatrix, minorMatrixSize);

            MPI_Send(vector, (minorMatrixSize * minorMatrixSize), MPI_INT, (i + 1), 99, MPI_COMM_WORLD);
            MPI_Send(&matrix[0][i], 1, MPI_INT, (i + 1), 99, MPI_COMM_WORLD);

            freeArray(triangleMatrix, minorMatrixSize);
            delete[] vector;
        }

        int result;
        int processesCount = defaultMatrixSize;
        int count = 1;

        while (count <= processesCount)
        {
            cout << "asking sources ... ";
            for (int i = 0; i < 4; i++)
            {
                if (!finishedProcesses[i])
                {
                    cout << (i + 1) << " ";
                }
            }
            cout << endl;
            for (int i = 0; i < defaultMatrixSize; i++)
            {
                if (!finishedProcesses[i])
                {
                    int flag = false;

                    // MPI_Probe((i + 1), 99, MPI_COMM_WORLD, &status);
                    MPI_Iprobe((i + 1), 99, MPI_COMM_WORLD, &flag, &status);

                    if (flag)
                    {
                        MPI_Recv(&result, 1, MPI_INT, i + 1, 99, MPI_COMM_WORLD, &status);

                        determinant += result;

                        count++;

                        finishedProcesses[i] = true;
                    }
                }
            }
        }

        cout << endl
             << "Determinant is: " << determinant << endl;
    }
    else
    {
        int *vector = new int[minorMatrixSize * minorMatrixSize];
        int a = 0;

        MPI_Recv(vector, (minorMatrixSize * minorMatrixSize), MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
        MPI_Recv(&a, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);

        int det = countVectorDet(vector);

        cout << "Rank " << rank << " current matrix: " << endl;
        for (int i = 0; i < minorMatrixSize * minorMatrixSize; i++)
        {
            if (i % 3 == 0)
            {
                cout << endl;
            }
            cout << vector[i] << "  ";
        }
        cout << endl;

        int answer = a * pow(-1, rank + 1) * det;

        cout << "Process rank " << rank << " determinant = " << a << " * (-1)**" << rank + 1 << " * " << det << " = " << answer << endl;

        MPI_Send(&answer, 1, MPI_INT, 0, 99, MPI_COMM_WORLD);

        delete[] vector;
    }

    MPI_Finalize();

    return 0;
}

int **formTriMatrix(int row, int col, int **matr)
{
    int **resultMatrix = new int *[3];

    for (int i = 0; i < minorMatrixSize; i++)
    {
        resultMatrix[i] = new int[minorMatrixSize];
    }

    int currentRow = 0;
    int currentCol = 0;

    for (int i = 0; i < defaultMatrixSize; i++)
    {
        for (int j = 0; j < defaultMatrixSize; j++)
        {

            if (i != row && j != col)
            {
                resultMatrix[currentRow][currentCol] = matr[i][j];

                currentCol++;

                if (currentCol == minorMatrixSize && currentRow != minorMatrixSize)
                {
                    currentCol = 0;
                    currentRow++;
                }
            }
        }
    }

    return resultMatrix;
}

int countVectorDet(int *v)
{
    int directTriangle = v[0] * v[4] * v[8] + v[1] * v[5] * v[6] + v[2] * v[3] * v[7];
    int reverseTriangle = v[2] * v[4] * v[6] + v[0] * v[5] * v[7] + v[1] * v[3] * v[8];

    return directTriangle - reverseTriangle;
}

void displayMatrix(int size, int **matrix)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            cout << matrix[i][j] << "   ";
        }
        cout << endl;
    }

    cout << endl;
}

void freeArray(int **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        delete[] matrix[i];
    }

    delete[] matrix;
}

int **allocate2DArray(int size)
{
    int **a = new int *[size];
    for (int i = 0; i < size; i++)
    {
        a[i] = new int[size];
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

int *toVector(int **matrix, int size)
{
    int *v = new int[size * size];

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            v[i * size + j] = matrix[i][j];
        }
    }

    return v;
}

int **readMatrixFromFile(string filename, int size)
{

    int **a = allocate2DArray(defaultMatrixSize);

    ifstream fp(filename);
    if (!fp)
    {
        cout << "Error, file couldn't be opened" << endl;
        exit(1);
    }
    for (int row = 0; row < size; row++)
    {
        for (int col = 0; col < size; col++)
        {
            fp >> a[row][col];
            if (!fp)
            {
                cout << "Error reading file for element " << row << "," << col << endl;
                exit(1);
            }
        }
    }
    fp.close();

    return a;
}