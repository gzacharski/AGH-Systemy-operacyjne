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

#define MAX_CLIENTS_NUMBER 64

const char *FILENAME = "golibroda.c";
const int PROJECT_ID = 'W';

enum barberState
{
    CHECKS,
    INVITING,
    SLEEPING,
    SHAVING
};

enum clientState
{
    BEING_SHAVED,
    CHECKING,
    WAITING,
    IDLE,
    LEAVING
};

enum boolean{
    YES,
    NO
};

struct clientToShave{
    pid_t clientPID;
    enum boolean isOnChair;
    enum boolean isShaved;
};

struct sharedData
{
    int semaphore;
    enum barberState currentBarberState;
    int maxNumberOfClients;
    int currentClientQuantity;
    pid_t clientsPID[MAX_CLIENTS_NUMBER];
    struct clientToShave clientToShave;
};

int initializeSempahore()
{
    key_t key = ftok(FILENAME, PROJECT_ID);
    if (key == -1)
    {
        perror("Can't get key");
        exit(EXIT_FAILURE);
    }

    int semaphoreId = semget(key, 1, 0666| IPC_CREAT | IPC_EXCL);
    if (semaphoreId == -1)
    {
        perror("Can't create semaphore ID\n");
        exit(EXIT_FAILURE);
    }
    
    int result=semctl(semaphoreId, 0, SETVAL, 1);
    if(result==-1){
        perror("Can't initialize semaphore\n");
        exit(EXIT_FAILURE);
    }

    return semaphoreId;
}

int takeSemaphore(int semaphoreID)
{
    struct sembuf sembuf;
    sembuf.sem_flg=0;
    sembuf.sem_num=0;
    sembuf.sem_op=-1;

    int result=semop(semaphoreID,&sembuf,1);
    if(result!=0){
        perror("Can't decrement semaphore...");
        exit(EXIT_FAILURE);
    }

    return result;
}

int releaseSemaphore(int semaphoreID)
{
    struct sembuf sembuf;
    sembuf.sem_flg=0;
    sembuf.sem_num=0;
    sembuf.sem_op=1;

    int result=semop(semaphoreID,&sembuf,1);
    if(result!=0){
        perror("Can't increment semaphore...");
        exit(EXIT_FAILURE);
    }

    return result;
}

int removeSempahore(int semaphoreID)
{
    int result = semctl(semaphoreID, 0, IPC_RMID);
    if (result == -1)
    {
        perror("Cant't remove semaphore...");
        exit(EXIT_FAILURE);
    }

    return result;
}

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
    int sharedBlockId = shmget(key, memorySize, 0666 | IPC_CREAT);
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