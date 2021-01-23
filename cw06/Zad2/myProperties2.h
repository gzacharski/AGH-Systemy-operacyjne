#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include <sys/sem.h>
// #include <sys/types.h>
// #include <sys/ipc.h>
// #include <sys/shm.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_CLIENTS_NUMBER 64
#define SEMAPHORE_NAME "/mysemaphore2"

const char *FILENAME = "golibroda.c";
const char *MEMORY_NAME= "/barberMemory";
//const char *SEMAPHORE_NAME= "/barberSemaphore2";
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
    sem_t* semaphore;
    enum barberState currentBarberState;
    int maxNumberOfClients;
    int currentClientQuantity;
    pid_t clientsPID[MAX_CLIENTS_NUMBER];
    struct clientToShave clientToShave;
};

sem_t* initializeSempahore()
{   
    sem_t *semaphoreId=sem_open(SEMAPHORE_NAME, O_CREAT, 0666,1);
    if(semaphoreId==SEM_FAILED){
        perror("Can't initialize semaphore\n");
        exit(EXIT_FAILURE);
    }

    return semaphoreId;
}

int takeSemaphore(sem_t *semaphoreID)
{
    if(semaphoreID==NULL){
        perror("Semaphore is null...");
        exit(EXIT_FAILURE);
    }

    //int result=sem_post(semaphoreID);
    int result=sem_wait(semaphoreID);
    if(result!=0){
        perror("Can't decrement semaphore...");
        exit(EXIT_FAILURE);
    }

    return result;
}

int releaseSemaphore(sem_t *semaphoreID)
{
    if(semaphoreID==NULL){
        perror("Semaphore is null...");
        exit(EXIT_FAILURE);
    }

    //int result=sem_wait(semaphoreID);
    int result=sem_post(semaphoreID);
    if(result!=0){
        perror("Can't increment semaphore...");
        exit(EXIT_FAILURE);
    }

    return result;
}

int removeSempahore(sem_t *semaphoreID)
{
    int result=sem_close(semaphoreID);
    if (result == -1)
    {
        perror("Cant't remove semaphore...");
        exit(EXIT_FAILURE);
    }

    return result;
}

int getSharedMemoryFD()
{
    int memorySize = sizeof(struct sharedData);

    int sharedBlockFD=shm_open(MEMORY_NAME, O_RDWR | O_CREAT, 0666);
    if (sharedBlockFD== -1)
    {
        perror("Can't get shared block fd.");
        exit(EXIT_FAILURE);
    }

    if(ftruncate(sharedBlockFD,memorySize)==-1){
        perror("Can't set segment size");
        exit(EXIT_FAILURE);
    }

    return sharedBlockFD;
}

struct sharedData *attachMemoryBlock()
{
    int sharedBlockFD = getSharedMemoryFD();

    int memorySize = sizeof(struct sharedData);

    struct sharedData *memoryBlock=mmap(NULL,memorySize, PROT_READ | PROT_WRITE, MAP_SHARED,sharedBlockFD,0);
    if (memoryBlock == NULL)
    {
        perror("Can't attach shared block.");
        exit(EXIT_FAILURE);
    }

    return memoryBlock;
}

int detachMemoryBlock(struct sharedData* filename)
{
    int memorySize = sizeof(struct sharedData);

    int result=munmap(filename, memorySize);
    if (result == -1)
    {
        perror("Can't detach memory block.");
        exit(EXIT_FAILURE);
    }

    return result;
}

int destroyMemoryBlock()
{
    int result=shm_unlink(MEMORY_NAME);
    if (result == -1)
    {
        perror("Can't destroy memory block.");
        exit(EXIT_FAILURE);
    }

    return result;
}