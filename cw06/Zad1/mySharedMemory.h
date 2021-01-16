#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include <time.h>
#include "myProperties.h"

struct sharedData
{
    int semaphore;
    enum barberState currentBarberState;
    int currentClientQuantity;
    pid_t clientsPID[MAX_CLIENTS_NUMBER];
};

int getSharedMemoryID()
{
    //request a key
    key_t key = ftok(FILENAME, PROJECT_ID);
    if (key == -1)
    {
        perror("Can't get key");
        exit(EXIT_FAILURE);
    }

    int memorySize = sizeof(struct sharedData);

    //get shared block/create it if it doesn't exist
    int sharedBlockId = shmget(key, memorySize, 0666 | IPC_CREAT | IPC_EXCL);
    if (sharedBlockId == -1)
    {
        perror("Can't get shared block id.");
        exit(EXIT_FAILURE);
    }

    return sharedBlockId;
}

struct sharedData *attachMemoryBlock()
{
    //get memoryID by filename and memorysize
    int sharedBlockID = getSharedMemoryID();

    //map the shared block into this process's memory
    //and give me a pointer to it
    struct sharedData *memoryBlock = shmat(sharedBlockID, NULL, 0);
    if (memoryBlock == NULL)
    {
        perror("Can't attach shared block.");
        exit(EXIT_FAILURE);
    }

    return memoryBlock;
}

int detachMemoryBlock(struct sharedData* filename)
{
    int result = shmdt(filename);
    if (result == -1)
    {
        perror("Can't detach memory block.");
        exit(EXIT_FAILURE);
    }

    return result;
}

int destroyMemoryBlock()
{
    int shared_block_id = getSharedMemoryID();

    int result = shmctl(shared_block_id, IPC_RMID, NULL);
    if (result == -1)
    {
        perror("Can't destroy memory block.");
        exit(EXIT_FAILURE);
    }

    return result;
}

