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
    int id;
    char type; // 'P' | 'V' | 'A' | 'R'
    int ts;
    int data[DATASIZE];
    int rank;
    int globalRank;
} msg;

int generateRandomNumber(int min, int max);
bool checkIsRankSent(bool *ranks, int rank);
bool anyUnsentRanksLeft(bool *sentRanks, int size);
int maxTimestamp(int ls, int ts);
bool compareByTimestamp(const message &a, const message &b);

int main(int argc, char *argv[])
{
    int rank, size, resRank;
    int contactRank = -1;

    bool isInResource = false;
    bool isMessagePassing = false;
    int root = -1;
    int sentRanksCount = -1;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Status status;
    MPI_Request messageRequest = MPI_REQUEST_NULL;

    MPI_Comm res_comm, proc_comm;
    MPI_Group group_world, res_group, proc_group;

    int resRanks[1];
    int ranks[size - 1];

    resRanks[0] = 0;
    bool recvRanks[3] = {false, false, false};

    for (int i = 1; i < size; i++)
    {
        ranks[i - 1] = i;
    }

    MPI_Comm_group(MPI_COMM_WORLD, &group_world);

    MPI_Group_incl(group_world, 1, resRanks, &res_group);
    MPI_Group_incl(group_world, size - 1, ranks, &proc_group);

    MPI_Comm_create(MPI_COMM_WORLD, res_group, &res_comm);
    MPI_Comm_create(MPI_COMM_WORLD, proc_group, &proc_comm);

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

    int processes[size - 1];
    bool sent[size - 1];
    for (int i = 0; i < size - 1; ++i)
    {
        processes[i] = i;
        sent[i] = false;
    }

    bool shouldShutdown = false;

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_MESSAGE);
    MPI_Type_commit(&MPI_MESSAGE);

    if (rank == RESOURCE)
    {
        int output = -1;
        // MPI_Comm_rank(res_comm, &resRank);
        while (!shouldShutdown)
        {
            message req;

            MPI_Irecv(&req, 1, MPI_MESSAGE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &messageRequest);
            MPI_Wait(&messageRequest, &status);
            if (req.id)
            {
                cout << "Resource received request! from rank " << req.rank << endl;
                int sum = 0;
                for (int i = 0; i < DATASIZE; ++i)
                {
                    cout << req.data[i] << "    ";
                    sum += req.data[i];
                }

                req.data[0] = sum;

                MPI_Isend(&req, 1, MPI_MESSAGE, req.rank, req.id, MPI_COMM_WORLD, &messageRequest);
            }
        }
    }

    if (rank != RESOURCE)
    {
        // MPI_Comm_rank(proc_comm, &rank);
        // MPI_Comm_size(proc_comm, &size);

        vector<message> queue;
        message msg;
        msg.id = -1;
        int ls = 0;
        int S = 1; // 1 - free, 0 - busy

        while (!shouldShutdown)
        {
            sleep(1);
            if (!isMessagePassing)
            {
                if (sentRanksCount != size - 2)
                {
                    ++sentRanksCount;
                    root = processes[sentRanksCount];
                    if (rank == root)
                    {
                        msg.globalRank = rank;
                        msg.rank = rank;
                        msg.id = size * rank + rank + 1;
                        msg.ts = ls;
                        msg.type = 'P';
                        for (int i = 0; i < DATASIZE; ++i)
                        {
                            msg.data[i] = (rank + 1) * size;
                        }

                        ls++;
                        isMessagePassing = true;
                        queue.push_back(msg);
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

                        cout << "Bcasting message from rank #" << rank << endl;
                    }

                    MPI_Bcast(&msg, 1, MPI_MESSAGE, root, MPI_COMM_WORLD);
                }
            }
            if (msg.id != -1 && rank != root && rank != RESOURCE)
            {
                sent[root] = true;
                ls = maxTimestamp(ls, (msg.ts + 1));
                ++ls;
                if (msg.type == 'P')
                {
                    S = 0;
                    queue.push_back(msg);
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
                    ask.type = 'A';

                    // cout << "Rank #" << rank << " received P operation from " << root << endl;

                    MPI_Isend(&ask, 1, MPI_MESSAGE, root, ask.id, MPI_COMM_WORLD, &messageRequest);
                }
                if (msg.type == 'V')
                {
                    S = 1;
                }
            }
            if (rank == root)
            {
                int count = 0;
                while (count < size - 2)
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
            if (queue.size() && S == 0 && !isInResource)
            {
                cout << "Rank #" << rank << "QUEUE STATE : ";
                for (int i = 0; i < queue.size(); ++i)
                {
                    cout << queue[i].rank << "  ";
                }
                cout << endl;
                message request = queue[0];
                queue.erase(queue.begin());
                if (request.rank == rank)
                {
                    isInResource = true;
                    contactRank = rank;
                    MPI_Isend(&request, 1, MPI_MESSAGE, RESOURCE, request.id, MPI_COMM_WORLD, &messageRequest);
                }
            }
            if (rank == contactRank)
            {
                message response;
                MPI_Irecv(&response, 1, MPI_MESSAGE, RESOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &messageRequest);
                MPI_Wait(&messageRequest, &status);
                cout << "rank " << rank << " got response from resource! " << response.data[0] << endl;
                isInResource = false;

                message open;
                open.rank = rank;
                open.globalRank = rank;
                open.id = rank + size;
                open.type = 'V';

                MPI_Bcast(&open, 1, MPI_MESSAGE, contactRank, MPI_COMM_WORLD);

                contactRank = -1;
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

bool checkIsRankSent(bool *ranks, int rank)
{
    return ranks[rank];
}

bool anyUnsentRanksLeft(bool *sentRanks, int size)
{
    for (int i = 0; i < size; ++i)
    {
        if (!sentRanks[i])
        {
            return true;
        }
    }

    return false;
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