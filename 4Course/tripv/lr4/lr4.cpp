#include <iostream>
#include <time.h>
#include "mpi.h"
#include <unistd.h>
#include <vector>
#include <cstdlib>

using namespace std;

#define DATASIZE 10
#define STRUCT_SIZE 5
#define CLIENT 1
#define RESOURCE 0

typedef struct message
{
    int source;
    int clientId;
    int clientRank;
    int data[10];
    int isMax;
} msg;

int countMax(int data[DATASIZE]);
int countMin(int data[DATASIZE]);

int main(int argc, char *argv[])
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Status status;
    MPI_Request messageRequest = MPI_REQUEST_NULL;

    // Создание типа для структуры MESSAGE
    const int nitems = STRUCT_SIZE;
    int blocklengths[STRUCT_SIZE] = {1, 1, 1, DATASIZE, 1};
    MPI_Datatype types[STRUCT_SIZE] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Datatype MPI_MESSAGE;
    MPI_Aint offsets[STRUCT_SIZE];

    offsets[0] = offsetof(msg, source);
    offsets[1] = offsetof(msg, clientId);
    offsets[2] = offsetof(msg, clientRank);
    offsets[3] = offsetof(msg, data);
    offsets[4] = offsetof(msg, isMax);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_MESSAGE);
    MPI_Type_commit(&MPI_MESSAGE);

    const int server = 0;
    const int res = 1;

    vector<message> queue;

    if (rank == server)
    {
        message msg;

        int flag = 0;
        bool isEmpty = true;
        int waitLimit = 0;

        while (true)
        {
            sleep(1);

            if (isEmpty && !queue.empty())
            {
                waitLimit = 0;
                cout << "sending to resource ..." << endl;
                isEmpty = false;
                message msg = queue[0];
                MPI_Isend(&msg, 1, MPI_MESSAGE, res, msg.clientId, MPI_COMM_WORLD, &messageRequest);
            }

            cout << "Catching messages ... " << endl;

            MPI_Irecv(&msg, 1, MPI_MESSAGE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &messageRequest);
            MPI_Wait(&messageRequest, &status);

            cout << endl
                 << "Queue state: ";
            for (int i = 0; i < queue.size(); ++i)
            {
                cout << queue[i].clientId << "   ";
            }
            cout << endl;

            waitLimit = 0;

            if (msg.source == RESOURCE)
            {
                isEmpty = true;

                if (msg.isMax)
                {
                    cout << endl
                         << "Server received max value equal to " << msg.data[0] << " by client id = " << msg.clientId << endl;
                }
                else
                {
                    cout << endl
                         << "Server received min value equal to " << msg.data[0] << " by client id = " << msg.clientId << endl;
                }
                cout << endl;

                queue.erase(queue.begin());

                MPI_Isend(&msg, 1, MPI_MESSAGE, msg.clientRank, msg.clientId, MPI_COMM_WORLD, &messageRequest);
            }

            if (msg.source == CLIENT)
            {
                queue.push_back(msg);
            }
        }

        // TODO: придумать, как останавливать работу сервера

        cout << "Server stopped working!" << endl;
        MPI_Abort(MPI_COMM_WORLD, 0);
    }
    else if (rank == res)
    {
        message msg;

        int waitLimit = 0;
        int flag = 1;

        while (true)
        {
            MPI_Irecv(&msg, 1, MPI_MESSAGE, server, MPI_ANY_TAG, MPI_COMM_WORLD, &messageRequest);

            MPI_Wait(&messageRequest, &status);

            for (int i = 0; i < DATASIZE; ++i)
            {
                cout << msg.data[i] << "    ";
            }
            cout << endl;

            if (msg.isMax)
            {
                msg.data[0] = countMax(msg.data);
            }
            else
            {
                msg.data[0] = countMin(msg.data);
            }

            msg.source = RESOURCE;

            MPI_Isend(&msg, 1, MPI_MESSAGE, server, msg.clientId, MPI_COMM_WORLD, &messageRequest);
        }
    }
    else
    {
        message msg;

        msg.source = CLIENT;
        msg.clientId = rand() + rank;
        msg.clientRank = rank;
        msg.isMax = rank % 2;

        for (int i = 0; i < DATASIZE; i++)
        {
            msg.data[i] = rand() + rank;
        }

        MPI_Isend(&msg, 1, MPI_MESSAGE, server, msg.clientId, MPI_COMM_WORLD, &messageRequest);

        MPI_Irecv(&msg, 1, MPI_MESSAGE, server, msg.clientId, MPI_COMM_WORLD, &messageRequest);

        MPI_Wait(&messageRequest, &status);

        cout << "Client #" << rank - 1 << " finished working!" << endl;
    }

    MPI_Type_free(&MPI_MESSAGE);

    MPI_Finalize();

    return 0;
}

int countMax(int data[DATASIZE])
{
    int max = -1;

    for (int i = 0; i < DATASIZE; ++i)
    {
        if (data[i] > max)
        {
            max = data[i];
        }
    }

    return max;
}

int countMin(int data[DATASIZE])
{
    int min = data[0];

    for (int i = 0; i < DATASIZE; ++i)
    {
        if (data[i] < min)
        {
            min = data[i];
        }
    }

    return min;
}