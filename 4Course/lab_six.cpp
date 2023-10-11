
//
#include <iostream>
using namespace std;
const int m = 4, n = 5;

double A1[m][m] = {
    {1, 0.3, 1.6, 2},
    {3.3, 1, 8.8, 3.9},
    {0.6, 0.1, 1, 1.4},
    {0.5, 0.3, 0.7, 1}};

double A1_copy[m][m] = {
    {1, 0.3, 1.6, 2},
    {3.3, 1, 8.8, 3.9},
    {0.6, 0.1, 1, 1.4},
    {0.5, 0.3, 0.7, 1}};

double A21[n][n] = {
    {1, 2, 1, 3, 1},
    {1.0 / 2.0, 1, 1.0 / 2.0, 3.0 / 2.0, 1.0 / 2.0},
    {1, 2, 1, 3, 1},
    {1.0 / 3.0, 2.0 / 3.0, 1.0 / 3.0, 1, 1.0 / 3.0},
    {1, 2, 1, 3, 1}},
       A21_copy[n][n] = {{1, 2, 1, 3, 1}, {1.0 / 2.0, 1, 1.0 / 2.0, 3.0 / 2.0, 1.0 / 2.0}, {1, 2, 1, 3, 1}, {1.0 / 3.0, 2.0 / 3.0, 1.0 / 3.0, 1, 1.0 / 3.0}, {1, 2, 1, 3, 1}},

       A22[n][n] = {{1, 2, 2, 4, 3}, {1.0 / 2.0, 1, 2.0 / 2.0, 4.0 / 2.0, 3.0 / 2.0}, {1.0 / 2.0, 2.0 / 2.0, 1, 2, 3.0 / 2.0}, {1.0 / 4.0, 2.0 / 4.0, 1.0 / 2.0, 1, 3.0 / 4.0}, {1.0 / 3.0, 2.0 / 3.0, 2.0 / 3.0, 4.0 / 3.0, 1}}, A22_copy[n][n] = {{1, 2, 2, 4, 3}, {1.0 / 2.0, 1, 2.0 / 2.0, 4.0 / 2.0, 3.0 / 2.0}, {1.0 / 2.0, 2.0 / 2.0, 1, 2, 3.0 / 2.0}, {1.0 / 4.0, 2.0 / 4.0, 1.0 / 2.0, 1, 3.0 / 4.0}, {1.0 / 3.0, 2.0 / 3.0, 2.0 / 3.0, 4.0 / 3.0, 1}}, A23[n][n] = {{1, 1.0 / 2.0, 1.0 / 3.0, 1.0 / 2.0, 1.0 / 4.0}, {2, 1, 2.0 / 3.0, 1, 1.0 / 2.0}, {3, 3.0 / 2.0, 1, 3.0 / 2.0, 3.0 / 4.0}, {2, 1, 2.0 / 3.0, 1, 1.0 / 2.0}, {4, 2, 4.0 / 3.0, 2, 1}}, A23_copy[n][n] = {{1, 1.0 / 2.0, 1.0 / 3.0, 1.0 / 2.0, 1.0 / 4.0}, {2, 1, 2.0 / 3.0, 1, 1.0 / 2.0}, {3, 3.0 / 2.0, 1, 3.0 / 2.0, 3.0 / 4.0}, {2, 1, 2.0 / 3.0, 1, 1.0 / 2.0}, {4, 2, 4.0 / 3.0, 2, 1}}, A24[n][n] = {{1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}}, A24_copy[n][n] = {{1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}};

double c1[m] = {0, 0, 0, 0};

double W1[m] = {0, 0, 0, 0}, sum_column1[m] = {0, 0, 0, 0}, sum_row1[m] = {0, 0, 0, 0}, W_itog[m] = {0, 0, 0, 0}, sum = 0, sobstven, is;
int i, j;

double metod1(double A[5][5], double A_copy[5][5], double w[5], int index)
{
    double wt[n] = {0, 0, 0, 0, 0}, sum_column[n] = {0, 0, 0, 0, 0}, sum_row[n] = {0, 0, 0, 0, 0}, c[n] = {0, 0, 0, 0, 0};
    for (j = 0; j < n; j++)
    {
        for (i = 0; i < n; i++)
        {
            sum_column[j] = sum_column[j] + A[i][j];
        }
    }
    for (j = 0; j < n; j++)
    {
        for (i = 0; i < n; i++)
        {
            A[i][j] = A[i][j] / sum_column[j];
        }
    }
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            sum_row[i] = sum_row[i] + A[i][j];
        }
        w[i] = sum_row[i] / n;
    }
    sum = 0;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            c[i] += A_copy[i][j] * w[i];
        }
        wt[i] = c[i] / w[i];
        sum += wt[i];
    }
    sobstven = sum / n;
    cout << "����������� �������� ������� �2" << index << " : " << sobstven << endl;
    is = (sobstven - n) / (n - 1);
    cout << "������ ���������������: " << is << endl;
    cout << endl;
    for (i = 0; i < n; i++)
    {
        return w[i];
    }
}

int main()
{
    setlocale(LC_ALL, "Russian");

    for (j = 0; j < m; j++)
    {
        for (i = 0; i < m; i++)
        {
            sum_column1[j] = sum_column1[j] + A1[i][j];
        }
    }

    for (j = 0; j < m; j++)
    {
        for (i = 0; i < m; i++)
        {
            A1[i][j] = A1[i][j] / sum_column1[j];
        }
    }

    for (i = 0; i < m; i++)
    {
        for (j = 0; j < m; j++)
        {
            sum_row1[i] = sum_row1[i] + A1[i][j];
        }
        W1[i] = sum_row1[i] / m;
    }

    cout << endl;

    sum = 0;

    for (i = 0; i < m; i++)
    {
        for (j = 0; j < m; j++)
        {
            c1[i] += A1_copy[i][j] * W1[i];
        }
        W_itog[i] = c1[i] / W1[i];
        sum += W_itog[i];
    }

    sobstven = sum / m;
    cout << "����������� �������� ������� �1: " << sobstven << endl;
    is = (sobstven - m) / (m - 1);
    cout << "������ ���������������: " << is << endl;
    cout << endl;
    double w21[n] = {0, 0, 0, 0, 0}, w22[n] = {0, 0, 0, 0, 0}, w23[n] = {0, 0, 0, 0, 0}, w24[n] = {0, 0, 0, 0, 0};
    double wp1[m], wp2[m], wp3[m], wp4[m], wp5[m];

    metod1(A21, A21_copy, w21, 1);
    metod1(A22, A22_copy, w22, 2);
    metod1(A23, A23_copy, w23, 3);
    metod1(A24, A24_copy, w24, 4);

    for (i = 0; i < m - 3; i++)
    {
        wp1[i] = w21[i];
        wp1[i + 1] = w22[i];
        wp1[i + 2] = w23[i];
        wp1[i + 3] = w24[i];
    }
    for (i = 0; i < m - 3; i++)
    {
        wp2[i] = w21[i + 1];
        wp2[i + 1] = w22[i + 1];
        wp2[i + 2] = w23[i + 1];
        wp2[i + 3] = w24[i + 1];
    }
    for (i = 0; i < m - 3; i++)
    {
        wp3[i] = w21[i + 2];
        wp3[i + 1] = w22[i + 2];
        wp3[i + 2] = w23[i + 2];
        wp3[i + 3] = w24[i + 2];
    }
    for (i = 0; i < m - 3; i++)
    {
        wp4[i] = w21[i + 3];
        wp4[i + 1] = w22[i + 3];
        wp4[i + 2] = w23[i + 3];
        wp4[i + 3] = w24[i + 3];
    }

    wp5[0] = w21[4];
    wp5[1] = w22[4];
    wp5[2] = w23[4];
    wp5[3] = w24[4];

    /*
    for (i = 0; i < m; i++) {
        cout << wp1[i] << " ";
    }
    cout << endl;
    for (i = 0; i < m; i++) {
        cout << wp2[i] << " ";
    }
    cout << endl;
    for (i = 0; i < m; i++) {
        cout << wp3[i] << " ";
    }
    cout << endl;
    for (i = 0; i < m; i++) {
        cout << wp4[i] << " ";
    }
    cout << endl;
    for (i = 0; i < m; i++) {
        cout << wp5[i] << " ";
    }
    cout << endl;
    */
    double D[n] = {0, 0, 0, 0, 0};

    for (i = 0; i < m; i++)
    {
        D[0] += wp1[i] * sum_column1[i];
        D[1] += wp2[i] * sum_column1[i];
        D[2] += wp3[i] * sum_column1[i];
        D[3] += wp4[i] * sum_column1[i];
        D[4] += wp5[i] * sum_column1[i];
    }

    for (i = 0; i < n; i++)
    {
        cout << "D" << i + 1 << " = " << D[i] << endl;
        ;
    }
    cout << endl;

    double max = 0;
    int ind = 0;
    for (i = 0; i < n; i++)
    {
        if (D[i] > max)
        {
            max = D[i];
            ind = i;
        }
    }
    cout << endl;
    cout << "�����������  D" << ind + 1 << " = " << max << endl;
}
