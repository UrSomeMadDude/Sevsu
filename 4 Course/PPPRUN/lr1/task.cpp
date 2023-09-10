#include <iostream>
#include <stdio.h>
#include <fstream>

using namespace std;

int **readMatrixFromFile(string filename, int rows, int cols);
int **allocate2DArray(int rows, int cols);
void freeArray(int **matrix, int size);
void displayMatrix(int rows, int cols, int **matrix);
double countMax(int currentRow, int cols, int **matrix);
double countMin(int currentRow, int cols, int **matrix);

int main(int argc, char *argv[])
{
    int rowsByDefault = 4;
    int colsByDefaut = 4;
    double q = 0.3;

    double *z = new double[rowsByDefault];
    for (int i = 0; i < rowsByDefault; i++)
    {
        z[i] = 0;
    }

    int **matrix = readMatrixFromFile("data.txt", rowsByDefault, colsByDefaut);

    cout << endl
         << "Матрица платежей: " << endl
         << endl;
    displayMatrix(rowsByDefault, colsByDefaut, matrix);

    for (int i = 0; i < rowsByDefault; i++)
    {
        double currentMax = countMax(i, colsByDefaut, matrix);
        double currentMin = countMin(i, colsByDefaut, matrix);

        double Zi = q * currentMin + (1 - q) * currentMax;

        cout << "Z" << (i + 1) << " = " << Zi << endl;

        z[i] = Zi;
    }

    int maxIndex = 0;
    double maxZ = z[maxIndex];

    for (int i = 1; i < rowsByDefault; i++)
    {
        if (z[i] > maxZ)
        {
            maxZ = z[i];
            maxIndex = i;
        }
    }

    cout << endl
         << "Значение Zmax = " << maxZ << endl;
    cout << endl
         << "Наилучшей является стратегия № " << (maxIndex + 1) << endl;

    freeArray(matrix, rowsByDefault);
    delete[] z;

    return 0;
}

int **readMatrixFromFile(string filename, int rows, int cols)
{

    int **a = allocate2DArray(rows, cols);

    ifstream fp(filename);
    if (!fp)
    {
        cout << "Error, file couldn't be opened" << endl;
        exit(1);
    }
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
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

int **allocate2DArray(int rows, int cols)
{
    int **a = new int *[rows];
    for (int i = 0; i < rows; i++)
    {
        a[i] = new int[cols];
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            a[i][j] = 0;
        }
    }

    return a;
}

void displayMatrix(int rows, int cols, int **matrix)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
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

double countMax(int currentRow, int cols, int **matrix)
{
    double max = matrix[currentRow][0];

    for (int i = 1; i < cols; i++)
    {
        if (matrix[currentRow][i] > max)
        {
            max = matrix[currentRow][i];
        }
    }

    return max;
}

double countMin(int currentRow, int cols, int **matrix)
{
    double min = matrix[currentRow][0];

    for (int i = 1; i < cols; i++)
    {
        if (matrix[currentRow][i] < min)
        {
            min = matrix[currentRow][i];
        }
    }

    return min;
}