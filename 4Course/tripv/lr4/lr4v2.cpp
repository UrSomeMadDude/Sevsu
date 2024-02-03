#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <vector>
#include <unistd.h>

using namespace std;

#define DATASIZE 10     // message array size
#define STRUCT_SIZE 5   // amount of fields oin structure
#define SERVER_RANK 3   // server
#define RESOURCES_NUM 3 // resources in the program
#define CLIENTS_NUM 8   // clients in the program

// the program requires 12 processes to run now
// total processes = CLIENTS_NUM + RESOURCES_NUM + 1 (RESOURCE)

typedef struct message
{
    int id;                // process id
    int res_num;           // resource number - basically it's rank (0, 1, or 2)
    int rank;              // process rank
    char operation;        // 'P' or 'V' or 'R'
    int message[DATASIZE]; // message
} msg;

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get current rank
    MPI_Comm_size(MPI_COMM_WORLD, &size); // get total ranks

    MPI_Status status;
    MPI_Request request = MPI_REQUEST_NULL;

    // Create message datatype for MPI
    const int nitems = STRUCT_SIZE;
    int blocklengths[STRUCT_SIZE] = {1, 1, 1, 1, DATASIZE};
    MPI_Datatype types[STRUCT_SIZE] = {MPI_INT, MPI_INT, MPI_CHAR, MPI_INT, MPI_INT};
    MPI_Datatype MPI_MESSAGE;
    MPI_Aint offsets[STRUCT_SIZE];

    offsets[0] = offsetof(msg, id);
    offsets[1] = offsetof(msg, res_num);
    offsets[2] = offsetof(msg, operation);
    offsets[3] = offsetof(msg, rank);
    offsets[4] = offsetof(msg, message);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_MESSAGE);
    MPI_Type_commit(&MPI_MESSAGE);

    vector<message> *queue = new vector<message>[RESOURCES_NUM]; // queue - array of vectors
    bool *busyRes = new bool[RESOURCES_NUM];                     // flags array
    for (int i = 0; i < RESOURCES_NUM; ++i)
    {                       // false - free
        busyRes[i] = false; // true - busy
    }

    bool clientsSatisfied = false; // not implemented - a flag to abort program process

    if (rank == SERVER_RANK)
    {
        message req;

        while (!clientsSatisfied)
        {
            sleep(1);
            //  cout << "Cathing messages ..." << endl;
            cout << "Queue state" << endl;
            for (int i = 0; i < RESOURCES_NUM; ++i)
            {
                cout << "Queue[" << i << "]: ";
                for (int j = 0; j < queue[i].size(); ++j)
                {
                    cout << queue[i][j].rank << " ";
                }
                cout << endl;
            }

            MPI_Irecv(&req, 1, MPI_MESSAGE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request); // get message
            MPI_Wait(&request, &status);

            if (req.operation == 'P') // if it's capture
            {
                if (busyRes[req.res_num])
                {
                    queue[req.res_num].push_back(req); // if requird resource is busy, push message to resource's queue
                    // cout << "Pushed to queue" << req.res_num << " message from " << req.rank << endl;
                }
                else
                {
                    MPI_Isend(&req, 1, MPI_MESSAGE, req.res_num, req.id, MPI_COMM_WORLD, &request);
                    // cout << "Sent request from " << req.rank << " to resource " << req.res_num << endl;
                    busyRes[req.res_num] = true;
                }
            }

            if (req.operation == 'V') // if it's "free" operation
            {
                if (queue[req.res_num].size()) // if queue isn't empty, pop the first element and send message to resource
                {
                    busyRes[req.res_num] = true;
                    message msg = queue[req.res_num][0];
                    queue[req.res_num].erase(queue[req.res_num].begin());
                    MPI_Isend(&msg, 1, MPI_MESSAGE, msg.res_num, msg.id, MPI_COMM_WORLD, &request);
                    // cout << "Sent request to resource which was taken from queue" << req.res_num << endl;
                }
                else
                {
                    busyRes[req.res_num] = false; // mark resource as available
                }
            }

            if (req.operation == 'R') // if resource mark operation
            {
                MPI_Isend(&req, 1, MPI_MESSAGE, req.rank, req.id, MPI_COMM_WORLD, &request); // send result to client
                // cout << "Sent resource response back to client " << req.rank << endl;
            }
        }
    }

    if (rank < SERVER_RANK) // if resource (0, 1 or 2)
    {
        message msg;
        while (!clientsSatisfied)
        {
            MPI_Irecv(&msg, 1, MPI_MESSAGE, SERVER_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, &request); // recv message
            MPI_Wait(&request, &status);

            for (int i = 0; i < DATASIZE; ++i)
            {
                msg.message[i] = msg.rank;
            }

            msg.operation = 'R';

            MPI_Isend(&msg, 1, MPI_MESSAGE, SERVER_RANK, 99, MPI_COMM_WORLD, &request); // send result back
        }
    }

    if (rank > SERVER_RANK) // if client (all rest)
    {
        message msg;
        msg.id = rank + 600;
        msg.operation = 'P';
        msg.rank = rank;
        msg.res_num = rank % RESOURCES_NUM;
        for (int i = 0; i < DATASIZE; ++i)
        {
            msg.message[i] = rank + 600;
        }

        MPI_Isend(&msg, 1, MPI_MESSAGE, SERVER_RANK, msg.id, MPI_COMM_WORLD, &request); // send capture operation
        cout << "Client[" << rank << "]: sent request to server" << endl;
        cout << "Its data: ";
        for (int i = 0; i < DATASIZE; ++i)
        {
            cout << msg.message[i] << " ";
        }
        cout << endl;

        MPI_Irecv(&msg, 1, MPI_MESSAGE, SERVER_RANK, msg.id, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);

        cout << "Client[" << rank << "]: Received response from server" << endl;
        cout << "Its data: ";
        for (int i = 0; i < DATASIZE; ++i)
        {
            cout << msg.message[i] << " ";
        }
        cout << endl;

        msg.operation = 'V';
        MPI_Isend(&msg, 1, MPI_MESSAGE, SERVER_RANK, msg.id, MPI_COMM_WORLD, &request); // send free operation
    }

    delete[] busyRes;
    delete[] queue;

    MPI_Finalize();

    return 0;
}