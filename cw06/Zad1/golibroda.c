#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include "myProperties.h"

struct sharedData* sharedData=NULL;
struct timespec currentTime;

void logBarberMessage(char* message){
    clock_gettime(CLOCK_MONOTONIC,&currentTime);
    printf("%ld.%.9ld - Barber %d: %s\n", 
        currentTime.tv_sec, 
        currentTime.tv_nsec,
        getpid(),
        message
    );
}

void cleanUp(){
    printf("Cleaning...\n");
    removeSempahore(sharedData->semaphore);
    detachMemoryBlock(sharedData);
    destroyMemoryBlock();
}

//handle SIGINT and SIGTEMR signals
void cleanUpAndTerminateProcess(int sigNum){
    printf("\nTerminating barber process: %d\n",getpid());
    cleanUp();
    kill(getpid(), SIGKILL);
}

int validateInputData(int argc, char *argv[]){
    int maxClientQuantity=0;

    if(argc!=2){
        printf("usage - %s [Liczba krzesel w poczeklani]\n",argv[0]);
        exit(EXIT_FAILURE);
    }else{
        maxClientQuantity=atoi(argv[1]);
        if(maxClientQuantity>MAX_CLIENTS_NUMBER){
            printf("Max numbe of clients is: %d.\n", MAX_CLIENTS_NUMBER);
            exit(EXIT_FAILURE);
        }
    }

    return maxClientQuantity;
}

void initializeBarber(int clientQuantity){
    printf("Started initializing barber...\n");

    //initialize shared memory and its semaphore
    sharedData=attachMemoryBlock();
    sharedData->currentClientQuantity=0;
    sharedData->maxNumberOfClients=clientQuantity;
    sharedData->currentBarberState=CHECKS;
    sharedData->semaphore=initializeSempahore();
    sharedData->clientToShave.clientPID=0;
    sharedData->clientToShave.isOnChair=NO;
    sharedData->clientToShave.isShaved=NO;

    printf("Barber initialized...\n");
    printf("Waiting for clients...\n");
}

void moveQueue(){
    if(sharedData->currentClientQuantity>0){
        
        for(int i=0; i<sharedData->currentClientQuantity; i++){
            sharedData->clientsPID[i]=sharedData->clientsPID[i+1];
        }
        sharedData->currentClientQuantity--;
    }
}

int main(int argc, char *argv[]){


    //handle signals
    if (signal(SIGINT, cleanUpAndTerminateProcess)  == SIG_ERR){
        printf("Can't terminate barber process\n");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTERM, cleanUpAndTerminateProcess) == SIG_ERR){
        printf("Can't terminate barber process\n");
        exit(EXIT_FAILURE);
    }

    int clientQuantity=validateInputData(argc,argv);

    printf("Number of max clients: %d\n",clientQuantity);
    
    //set up semaphores and shared memory
    initializeBarber(clientQuantity);

    while(1){
        takeSemaphore(sharedData->semaphore);

        switch (sharedData->currentBarberState)
        {
        case CHECKS:
            logBarberMessage("is checking the queue...");

            if(sharedData->currentClientQuantity==0){
                logBarberMessage("is going to sleep, because there is no client in the queue.");
                sharedData->currentBarberState=SLEEPING;
            }else{
                logBarberMessage("is inviting a client.");
                sharedData->currentBarberState=INVITING;
                sharedData->clientToShave.clientPID=sharedData->clientsPID[0];
                sharedData->clientToShave.isOnChair=NO;
                sharedData->clientToShave.isShaved=NO;
                printf("\t%d\n",sharedData->clientToShave.clientPID);
            }
            break;
        case INVITING:
            logBarberMessage("is waiting for a client to sit on chair.");
            if(sharedData->clientToShave.isOnChair==YES){
                moveQueue();
                sharedData->currentBarberState=SHAVING;
            }
            break;
        case SLEEPING:
            //logBarberMessage("is sleeping.");
            break;
        case SHAVING:
            if(sharedData->clientToShave.isOnChair==YES){
                logBarberMessage("is shaving.");
                printf("\tBarber is shaving client %d\n",sharedData->clientToShave.clientPID);
                sharedData->clientToShave.isShaved=YES;
            }
            break;
        default:
            break;
        }

        releaseSemaphore(sharedData->semaphore);
    }

    return 0;
}