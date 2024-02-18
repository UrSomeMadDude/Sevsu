#include <iostream>
#include <time.h>
#include "mpi.h"
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <algorithm>

using namespace std;

#define DATASIZE 5
#define STRUCT_SIZE 6
#define RESOURCE 3

typedef struct message
{
    int id;    // message id
    char type; // 'P' | 'V' | 'A' | 'R'
    int ts;    // timestapmp
    int data[DATASIZE];
    int rank; // process rank
} msg;

int maxTimestamp(int ls, int ts);                            // вычисление максимума из двух временных меток
bool compareByTimestamp(const message &a, const message &b); // функция фильтрации контейнера

int main(int argc, char *argv[])
{
    int rank, size;
    int contactRank = -1; // ранг, который сейчас взаимодействует с ресурсом

    bool isMessagePassing = false; // флаг отражабщий состояние, передаётся ли сейчас между процессами сообщение
    int root = -1;                 // ранг, который будет рассылать сообщения
    int sentRanksCount = -1;       // количество рангов, которые уже пораздавали сообщения

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Status status;
    MPI_Request messageRequest = MPI_REQUEST_NULL;

    // Создание типа для структуры MESSAGE
    const int nitems = STRUCT_SIZE;
    int blocklengths[STRUCT_SIZE] = {1, 1, 1, DATASIZE, 1, 1};
    MPI_Datatype types[STRUCT_SIZE] = {MPI_INT, MPI_CHAR, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Datatype MPI_MESSAGE;
    MPI_Aint offsets[STRUCT_SIZE];

    offsets[0] = offsetof(msg, id);
    offsets[1] = offsetof(msg, type);
    offsets[2] = offsetof(msg, ts);
    offsets[3] = offsetof(msg, data);
    offsets[4] = offsetof(msg, rank);

    int processes[size - 1]; // клиенты
    bool sent[size - 1];     // булев массив, показывающий, какие ранги отправили сообщения
    for (int i = 0; i < size - 1; ++i)
    {
        processes[i] = i;
        sent[i] = false;
    }

    bool shouldShutdown = false; // флаг на остановку работы

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_MESSAGE);
    MPI_Type_commit(&MPI_MESSAGE);

    if (rank == RESOURCE) // если ресурс
    {
        while (!shouldShutdown)
        {
            message req;

            MPI_Irecv(&req, 1, MPI_MESSAGE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &messageRequest);
            MPI_Wait(&messageRequest, &status); // получаем сообщение, тут блокируемся, пока не получим
            if (req.id)
            {
                cout << "Resource received request! from rank " << req.rank << endl;
                int sum = 0;
                for (int i = 0; i < DATASIZE; ++i)
                {
                    cout << req.data[i] << "    ";
                    sum += req.data[i];
                }

                req.data[0] = sum; // посчимтали максимум и положили в первый элемент массива data

                MPI_Isend(&req, 1, MPI_MESSAGE, req.rank, req.id, MPI_COMM_WORLD, &messageRequest); // отдали ответ тому, откуда прилетел запрос
            }
        }
    }

    if (rank != RESOURCE) // если клиент
    {

        vector<message> queue; // каждый процесс хранит очередь процессов
        message msg;
        msg.id = -1;
        int ls = 0; // ls инициируем нулём по умолчанию
        int S = 1;  // 1 - free, 0 - busy

        while (!shouldShutdown)
        {
            sleep(1);
            if (!isMessagePassing) // если обмена сообщениями не происходит
            {
                if (sentRanksCount != size - 2) // если ещё не все передали свои данные (по одному разу)
                {
                    ++sentRanksCount;
                    root = processes[sentRanksCount]; // этот ранг будет бродкастить сообщение
                    if (rank == root)                 // если ранг это бкастеро
                    {
                        msg.rank = rank;
                        msg.id = size * rank + rank + 1;
                        msg.ts = ls;
                        msg.type = 'P';
                        for (int i = 0; i < DATASIZE; ++i)
                        {
                            msg.data[i] = (rank + 1) * size;
                        }

                        ls++;                    // увеличили метку, так как выполнили действие
                        isMessagePassing = true; // говорим, что передаём сообьщение
                        queue.push_back(msg);    // пушим в очередь сообщение
                        if (queue.size() > 1)
                        {
                            std::sort(queue.begin(), queue.end(), compareByTimestamp); // если очередь непуста, то в неё пушим элемент, сортируя по ls, так в очереди первым будет самый актулаьный
                        }

                        cout << "Rank #" << rank << " QUEUE STATE: " << endl;
                        for (int i = 0; i < queue.size(); ++i)
                        {
                            cout << queue[i].rank << "  ";
                        }
                        cout << endl;

                        cout << "Bcasting message from rank #" << rank << endl;
                    }

                    MPI_Bcast(&msg, 1, MPI_MESSAGE, root, MPI_COMM_WORLD); // рассылка собщения остальным процессам
                }
            }
            if (msg.id != -1 && rank != root && rank != RESOURCE) // если существует рассылаемое сообщение и процесс это не ресурс и не тот, кто его разослал
            {
                sent[root] = true;                   // говорим, что этот ранг уже отправил сообщение, и больше не будет рассылать
                ls = maxTimestamp(ls, (msg.ts + 1)); // считаем временную метку
                ++ls;                                // увеличили ls
                if (msg.type == 'P')                 // если сообщение это захват ресурса
                {
                    S = 0;                // помечаем, что ресурс занят
                    queue.push_back(msg); // положили сообщение в локальную очередь
                    if (queue.size() > 1)
                    {
                        std::sort(queue.begin(), queue.end(), compareByTimestamp);
                    }

                    cout << "Rank #" << rank << " QUEUE STATE: " << endl;
                    for (int i = 0; i < queue.size(); ++i)
                    {
                        cout << queue[i].rank << "  ";
                    }
                    cout << endl;

                    message ask;
                    ask.rank = rank;
                    ask.id = msg.id;
                    ask.ts = msg.ts;
                    ask.type = 'A'; // помечаем сообщение как ASK для отдачи ответа рассыльщику

                    // cout << "Rank #" << rank << " received P operation from " << root << endl;

                    MPI_Isend(&ask, 1, MPI_MESSAGE, root, ask.id, MPI_COMM_WORLD, &messageRequest); // отправить сообщение рассыльщику
                }
                if (msg.type == 'V') // если сообщение это освобождение ресурса
                {
                    S = 1; // поставить флаг ресурса в СВОБОДЕН
                }
            }
            if (rank == root) // если мы это рассыльщик
            {
                int count = 0;
                while (count < size - 2) // получаем ASK-сообщение от двух дроугих клиентов
                {
                    message ask;
                    MPI_Irecv(&ask, 1, MPI_MESSAGE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &messageRequest);
                    MPI_Wait(&messageRequest, &status);
                    ++count;

                    // cout << "Rank #" << rank << " got ask message from rank #" << ask.rank << endl;
                }
                S = 0;
                isMessagePassing = false;
            }
            if (queue.size() && S == 0) // если очередь не пуста и роесурс занят
            {
                cout << "Rank #" << rank << "QUEUE STATE : ";
                for (int i = 0; i < queue.size(); ++i)
                {
                    cout << queue[i].rank << "  ";
                }
                cout << endl;
                message request = queue[0]; // получили из очереди голову
                queue.erase(queue.begin());
                if (request.rank == rank) // если ранг в голове равер текущему процессу
                {
                    contactRank = rank;                                                                         // ранг, контактирующий с ресурсом = rank
                    MPI_Isend(&request, 1, MPI_MESSAGE, RESOURCE, request.id, MPI_COMM_WORLD, &messageRequest); // отравляем ресурсу сообщение
                }
            }
            if (rank == contactRank) // если ранг это раг, который взаимодействует сейчас с ресурсом
            {
                message response;
                MPI_Irecv(&response, 1, MPI_MESSAGE, RESOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &messageRequest); // получили ответ от ресурса
                MPI_Wait(&messageRequest, &status);
                cout << "rank " << rank << " got response from resource! " << response.data[0] << endl;

                message open;
                open.rank = rank;
                open.id = rank + size;
                open.type = 'V'; // сменили операцию на освобождение ресурса

                MPI_Bcast(&open, 1, MPI_MESSAGE, contactRank, MPI_COMM_WORLD); // рассылка сообщения об освобождении всем клиентам

                contactRank = -1; // ставим ранг контактирующий с ресурсом в undefined
            }
        }
    }

    MPI_Type_free(&MPI_MESSAGE);

    MPI_Finalize();

    return 0;
}

int generateRandomNumber(int min, int max)
{
    return min + (rand() % static_cast<int>(max - min + 1));
}

int maxTimestamp(int ls, int ts)
{
    if (ls > (ts))
    {
        return ls;
    }

    return ts;
}

bool compareByTimestamp(const message &a, const message &b)
{
    return a.ts < b.ts;
}