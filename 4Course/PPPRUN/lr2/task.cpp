#include <iostream>
#include <stdio.h>
#include <fstream>

using namespace std;

const int e = 2;
const int z = 3;
const int q = 2;
const int a = q;

const double PQ1 = 0.55;
const double PQ2 = 0.45;

const int fictiveZ = 0;
const int inversedStrategy = 2;
int strategiesCount = 0;
double strategiesResults[4] = {0, 0, 0, 0};

const double U[e][z][a][q] = {
    {
        {
            {65, 60},
            {80, 80},
        },
        {
            {0, 0},
            {0, 0},
        },
        {
            {0, 0},
            {0, 0},
        },
    },
    {
        {
            {0, 0},
            {0, 0},
        },
        {
            {50, 58},
            {65, 65},
        },
        {
            {80, 90},
            {40, 68},
        },
    },
};

double **readMatrixFromFile(string filename, int rows, int cols);
double **allocate2DArray(int rows, int cols);
void freeArray(int **matrix, int size);

int main(int argc, char const *argv[])
{
    double ***m = new double **[e];
    for (int i = 0; i < e; ++i)
    {
        m[i] = new double *[z];
    }

    for (int i = 0; i < e; ++i)
    {
        for (int j = 0; j < z; ++j)
        {
            m[i][j] = new double[q];
        }
    }

    m[0] = readMatrixFromFile("experiment1.txt", z, q);
    m[1] = readMatrixFromFile("experiment2.txt", z, q);

    cout << "Стратегии: " << endl;

    for (int i = 0; i < e; ++i)
    {
        for (int j = 0; j < z; ++j)
        {
            for (int k = 0; k < q; ++k)
            {
                if (m[i][j][k])
                {
                    if (j == fictiveZ)
                    {
                        cout << "(e" << i << ", d" << i << k + 1 << "), d" << i << k + 1 << "(z" << j << ") = a" << k + 1 << endl;
                    }
                    else if (k + 1 == inversedStrategy)
                    {
                        if (j == 1)
                        {
                            cout << "(e" << i << ", d" << i << k + 1 << "), d" << i << k + 1 << "(z" << j << ") = a" << 2 << endl;
                        }

                        if (j == 2)
                        {
                            cout << "(e" << i << ", d" << i << k + 1 << "), d" << i << k + 1 << "(z" << j << ") = a" << 1 << endl;
                        }
                    }
                    else
                    {
                        cout << "(e" << i << ", d" << i << k + 1 << "), d" << i << k + 1 << "(z" << j << ") = a" << k + 1 << endl;
                    }
                }
            }
        }
    }

    cout << endl
         << "Оценки: " << endl;

    for (int i = 0; i < e; ++i)
    {
        for (int j = 0; j < z; ++j)
        {
            for (int k = 0; k < q; ++k)
            {
                for (int n = 0; n < a; ++n)
                {
                    if (U[i][j][n][k])
                    {
                        cout << "U(e" << i << ", z" << j << ", a" << n + 1
                             << ", q" << k + 1 << ") = " << U[i][j][n][k] << endl;
                    }
                }
            }
        }
    }

    for (int i = 0; i < e; ++i)
    {
        for (int j = 0; j < z; ++j)
        {
            for (int k = 0; k < q; ++k)
            {
                if (m[i][j][k] && j == fictiveZ)
                {
                    if (k == (q - 1))
                    {
                        ++strategiesCount;
                        double Pa1 = m[i][j][k];
                        double Pa2 = m[i][j][k - 1];

                        double U1 = U[i][j][k][k];
                        double U2 = U[i][j][k][k - 1];

                        cout << "Paq1 = " << Pa1 << endl;
                        cout << "Paq2 = " << Pa2 << endl;
                        cout << "U(e" << i << ", z" << j << ", a" << k + 1 << ", q" << k << ") = " << U1 << endl;
                        cout << "U(e" << i << ", z" << j << ", a" << k + 1 << ", q" << k + 1 << ") = " << U2 << endl;

                        double UQ1 = Pa1 * U1;
                        double UQ2 = Pa2 * U2;

                        double U = PQ1 * UQ1 + PQ2 * UQ2;

                        cout << "U = " << U << endl;
                        strategiesResults[strategiesCount - 1] = U;
                    }
                    else
                    {
                        ++strategiesCount;
                        double Pa1 = m[i][j][k];
                        double Pa2 = m[i][j][k + 1];

                        double U1 = U[i][j][k][k];
                        double U2 = U[i][j][k][k + 1];

                        cout << "Paq1 = " << Pa1 << endl;
                        cout << "Paq2 = " << Pa2 << endl;
                        cout << "U(e" << i << ", z" << j << ", a" << k + 1 << ", q" << k + 1 << ") = " << U1 << endl;
                        cout << "U(e" << i << ", z" << j << ", a" << k + 1 << ", q" << k + 2 << ") = " << U2 << endl;

                        double UQ1 = Pa1 * U1;
                        double UQ2 = Pa2 * U2;

                        double U = PQ1 * UQ1 + PQ2 * UQ2;

                        cout << "U = " << U << endl;

                        strategiesResults[strategiesCount - 1] = U;
                    }
                }
                else if (m[i][j][k])
                {
                    if (m[i][j][k] && j < z - 1 && k < q - 1)
                    {
                        ++strategiesCount;
                        cout << "STOPPED ON " << i << " " << j << " " << k << endl;
                        double Pa1q1 = m[i][j][k];
                        double Pa1q2 = m[i][j][k + 1];

                        double Pa2q1 = m[i][j + 1][k];
                        double Pa2q2 = m[i][j + 1][k + 1];

                        double U11 = U[i][j][k][k];
                        double U12 = U[i][j + 1][k + 1][k];

                        double U21 = U[i][j][k][k + 1];
                        double U22 = U[i][j + 1][k + 1][k + 1];

                        double UQ1 = Pa1q1 * U11 + Pa1q2 * U12;
                        double UQ2 = Pa2q1 * U21 + Pa2q2 * U22;

                        cout << "Pa1q1 = " << Pa1q1 << endl;
                        cout << "Pa1q2 = " << Pa1q2 << endl;

                        cout << "U(e" << i << ", z" << j << ", a" << k + 1 << ", q" << k + 1 << ") = " << U11 << endl;
                        cout << "U(e" << i << ", z" << j + 1 << ", a" << k + 2 << ", q" << k + 1 << ") = " << U12 << endl;

                        cout << "Pa2q1 = " << Pa2q1 << endl;
                        cout << "Pa2q2 = " << Pa2q2 << endl;

                        cout << "U(e" << i << ", z" << j << ", a" << k + 1 << ", q" << k + 2 << ") = " << U21 << endl;
                        cout << "U(e" << i << ", z" << j + 1 << ", a" << k + 2 << ", q" << k + 2 << ") = " << U22 << endl;

                        double US = PQ1 * UQ1 + PQ2 * UQ2;

                        cout << "U = " << US << endl;

                        strategiesResults[strategiesCount - 1] = US;

                        ++strategiesCount;

                        Pa2q1 = m[i][j][k];
                        Pa2q2 = m[i][j][k + 1];
                        Pa1q1 = m[i][j + 1][k];
                        Pa1q2 = m[i][j + 1][k + 1];

                        U11 = U[i][j + 1][k][k];
                        U21 = U[i][j][k + 1][k];
                        U12 = U[i][j + 1][k][k + 1];
                        U22 = U[i][j][k + 1][k + 1];

                        UQ1 = Pa1q1 * U11 + Pa2q1 * U21;
                        UQ2 = Pa1q2 * U12 + Pa2q2 * U22;

                        US = PQ1 * UQ1 + PQ2 * UQ2;

                        cout << "U = " << US << endl;

                        strategiesResults[strategiesCount - 1] = US;
                    }
                }
            }
        }
    }

    int max = strategiesResults[0];
    int index = 0;

    for (int i = 1; i < 4; ++i)
    {
        if (strategiesResults[i] > max)
        {
            max = strategiesResults[i];
            index = i;
        }
    }

    cout << "Макс. оценка = " << max << " и принадлежит стратегии " << index + 1 << endl;

    for (int i = 0; i < e; ++i)
    {
        for (int j = 0; j < z; ++j)
        {
            delete[] m[i][j];
        }
    }

    for (int i = 0; i < e; ++i)
    {
        delete[] m[i];
    }

    delete[] m;

    return 0;
}

double **readMatrixFromFile(string filename, int rows, int cols)
{
    double **a = allocate2DArray(rows, cols);

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

double **allocate2DArray(int rows, int cols)
{
    double **a = new double *[rows];
    for (int i = 0; i < rows; i++)
    {
        a[i] = new double[cols];
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