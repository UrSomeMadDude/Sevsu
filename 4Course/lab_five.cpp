// LR5.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
using namespace std;

const int row = 8, column = 5;

int matrix[row][column] = {
    /*K1 K2 K3 K4 K5*/
    /*x1*/ {3, 5, 5, 4, 4},
    /*x2*/ {4, 4, 4, 5, 4},
    /*x3*/ {5, 4, 3, 3, 5},
    /*x4*/ {3, 5, 3, 5, 3},
    /*x5*/ {4, 2, 4, 5, 5},
    /*x6*/ {3, 5, 3, 5, 3},
    /*x7*/ {5, 3, 4, 3, 4},
    /*x8*/ {4, 5, 3, 4, 3},
};

// Матрица степеней важности
int A[column][column] = {
    /*K1 K2 K3 K4 K5*/
    /*K1*/ {0, 0, 0, 2, 0},
    /*K2*/ {0, 0, 0, 0, 1},
    /*K3*/ {0, 0, 0, 3, 0},
    /*K4*/ {0, 2, 0, 0, 0},
    /*K5*/ {0, 0, 0, 0, 0}};

int i, j, k, element = 0, m = 1, N_model[column], colvo = 0, kol_row = row, kol_column = 0;
int sum[row] = {0, 0, 0, 0, 0, 0, 0, 0}, n = 0;
int index[row] = {-1, -1, -1, -1, -1, -1, -1, -1};

int NMODEL(int a)
{
    for (k = 0; k < column; k++)
    {
        if (A[a][k] != 0)
        {
            element += A[a][k];
            NMODEL(k);
        }
    }
    return N_model[column];
}

int main()
{
    setlocale(LC_ALL, "Russian");
    // Строим n-модель
    for (i = 0; i < column; i++)
    {
        colvo = 0;
        element = 0;
        for (j = 0; j < column; j++)
        {
            colvo++;

            if (A[i][j] != 0)
            {
                colvo = 0;
                element += A[i][j];
                NMODEL(j);
            }
            if (A[i][j] == 0 && colvo == column)
            {
                for (k = 0; k < column; k++)
                {
                    if (A[k][j] != 0)
                    {
                        NMODEL(k);
                    }
                }
            }
        }
        N_model[i] = element;
    }

    cout << "N модель" << endl;
    for (i = 0; i < column; i++)
    {
        cout << N_model[i] << " ";
    }
    cout << endl;
    // Закончили строить

    /*ОПРЕДЕЛЕНИЕ НЕ СРАВНИМЫХ РЕШЕНИЙ*/
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < column; j++)
        {
            sum[i] = sum[i] + matrix[i][j];
        }
    }

    for (k = 0; k < row; k++)
    {
        for (i = 0; i < row; i++)
        {
            if (sum[k] == sum[i] && k < i)
            {
                for (j = 0; j < column; j++)
                {
                    if (matrix[k][j] == matrix[i][j])
                    {
                        n++;
                    }
                }
                if (n == column)
                {
                    for (j = 0; j < column; j++)
                    {
                        matrix[i][j] = 0;
                    }
                    kol_row--;
                }
                n = 0;
            }
            if (sum[k] > sum[i] && k < i)
            {
                for (j = 0; j < column; j++)
                {
                    if (matrix[k][j] > matrix[i][j] || matrix[k][j] == matrix[i][j])
                    {
                        n++;
                    }
                }

                if (n == column)
                {
                    for (j = 0; j < column; j++)
                    {
                        matrix[i][j] = 0;
                    }
                    kol_row--;
                }
                n = 0;
            }
            if (sum[k] < sum[i] && k < i)
            {
                for (j = 0; j < column; j++)
                {
                    if (matrix[k][j] < matrix[i][j] || matrix[k][j] == matrix[i][j])
                    {
                        n++;
                    }
                }
                if (n == column)
                {
                    for (j = 0; j < column; j++)
                    {
                        matrix[k][j] = 0;
                    }
                    kol_row--;
                }
                n = 0;
            }
        }
    }

    /*КОНЕЦ ОПРЕДЕЛЕНИЯ НЕ СРАВНИМЫХ РЕШЕНИЙ*/
    cout << "Исходная матрица:" << endl;
    cout << "       K1 K2 K3 K4 K5" << endl;
    for (i = 0; i < row; i++)
    {
        cout << "X" << i + 1 << ": {  ";
        for (j = 0; j < column; j++)
        {
            cout << matrix[i][j] << "  ";
        }
        cout << "}" << endl;
    }
    cout << endl;

    for (i = 0; i < column; i++)
    {
        kol_column += N_model[i];
    }

    // Формируем модифицированные векторные оценки с учетом н модели
    int nm, c, e = 0;
    ;
    int **modif = new int *[row];
    for (k = 0; k < row; k++)
        modif[k] = new int[kol_column];

    for (i = 0; i < row; i++)
    {
        for (j = 0; j < kol_column; j++)
        {
            modif[i][j] = 0;
        }
    }

    // e = ;
    for (i = 0; i < row; i++)
    {
        e = -1;
        for (j = 0; j < column; j++)
        {
            nm = N_model[j];
            for (k = 0; k < nm; k++)
            {
                e++;
                for (c = e; c < kol_column; c++)
                {
                    modif[i][c] = matrix[i][j];
                }
            }
        }
    }

    cout << endl;
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < kol_column; j++)
        {
            cout << modif[i][j] << " ";
        }
        cout << endl;
    }

    int temp;

    for (int m = 0; m < (row * kol_column - 1); m++) // сдвиги очередных элементов в правильную позицию
        /*сдвиг элемента массива в правильную позицию*/
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < kol_column + 1; j++)
            {
                /*АНАЛИЗ НА ПОСЛЕДНИЙ ЭЛЕМЕНТ МАССИВА*/
                if (i == row - 1 && j == kol_column - 1)
                { // Если строка последняя и справа тупик, то ничего не делаем
                    continue;
                }
                /*КОНЕЦ АНАЛИЗА НА ПОСЛЕДНЮЮ СТРОКУ*/

                if (modif[i][j] < modif[i][j + 1])
                {                       // Если элемент не на своей позиции
                    temp = modif[i][j]; // Обмен местами
                    modif[i][j] = modif[i][j + 1];
                    modif[i][j + 1] = temp;
                }
            }
        }

    cout << endl;
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < kol_column; j++)
        {
            cout << modif[i][j] << " ";
        }
        cout << endl;
    }

    /*ОПРЕДЕЛЕНИЕ НЕ СРАВНИМЫХ РЕШЕНИЙ*/
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < kol_column; j++)
        {
            sum[i] = sum[i] + modif[i][j];
        }
    }

    for (k = 0; k < row; k++)
    {
        for (i = 0; i < row; i++)
        {
            if (sum[k] == sum[i] && k < i)
            {
                for (j = 0; j < kol_column; j++)
                {
                    if (modif[k][j] == modif[i][j])
                    {
                        n++;
                    }
                }
                if (n == kol_column)
                {
                    for (j = 0; j < kol_column; j++)
                    {
                        modif[i][j] = 0;
                    }
                    kol_row--;
                }
                n = 0;
            }
            if (sum[k] > sum[i] && k < i)
            {
                for (j = 0; j < kol_column; j++)
                {
                    if (modif[k][j] > modif[i][j] || modif[k][j] == modif[i][j])
                    {
                        n++;
                    }
                }

                if (n == kol_column)
                {
                    for (j = 0; j < kol_column; j++)
                    {
                        modif[i][j] = 0;
                    }
                    kol_row--;
                }
                n = 0;
            }
            if (sum[k] < sum[i] && k < i)
            {
                for (j = 0; j < kol_column; j++)
                {
                    if (modif[k][j] < modif[i][j] || modif[k][j] == modif[i][j])
                    {
                        n++;
                    }
                }
                if (n == kol_column)
                {
                    for (j = 0; j < kol_column; j++)
                    {
                        modif[k][j] = 0;
                    }
                    kol_row--;
                }
                n = 0;
            }
        }
    }

    cout << endl;
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < kol_column; j++)
        {
            if (modif[i][j] != 0)
            {
                index[i] = i;
            }
        }
    }

    /*КОНЕЦ ОПРЕДЕЛЕНИЯ НЕ СРАВНИМЫХ РЕШЕНИЙ*/
    cout << endl;
    for (i = 0; i < row; i++)
    {
        if (index[i] != -1)
        {
            cout << "X" << i + 1 << ": {  ";
            for (j = 0; j < kol_column; j++)
            {
                cout << modif[i][j] << "  ";
            }
            cout << "}" << endl;
        }
    }
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
