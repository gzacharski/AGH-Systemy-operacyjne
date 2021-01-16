#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include "myProperties.h"

struct sharedData* sharedData=NULL;
int clientQuantity;
int repetitionNumber;
//initial client state
enum clientState clientState=IDLE;
// struct timespec time
struct timespec currentTime;

void initializeClient(){
    sharedData=attachMemoryBlock();
}

void cleanUpBeforeShutdown(){
    detachMemoryBlock(sharedData);
}

void validateInputData(int argc, char *argv[]){

    if(argc!=3){
        printf("Usage - %s [Liczba klientow] [Liczba strzyzen]\n",argv[0]);
        exit(EXIT_FAILURE);
    }else{
        clientQuantity=atoi(argv[1]);
        repetitionNumber=atoi(argv[2]);
    }
}

void joinQueue(){
    sharedData->clientsPID[sharedData->currentClientQuantity]=getpid();
    sharedData->currentClientQuantity++;
}

void logClientMessage(char* message){
    clock_gettime(CLOCK_MONOTONIC,&currentTime);
    printf("%ld.%.9ld - Client %d: %s\n", 
        currentTime.tv_sec, 
        currentTime.tv_nsec,
        getpid(),
        message
    );
}

int main(int argc, char *argv[]){

    atexit(cleanUpBeforeShutdown);

    validateInputData(argc,argv);
    initializeClient();

    for(int i=1; i<clientQuantity; i++){
        pid_t pid=fork();
        if(pid<0){
            perror("Can't fork...");
            exit(EXIT_FAILURE);
        }else if(pid==0){
            //child process
        }else{
            //parent process
            break;
        }
    }

    clientState=CHECKING;

    //number of visiting barber and being shaved
    int i=0;
    while(i<repetitionNumber){
        takeSemaphore(sharedData->semaphore);

        switch (clientState)
        {
        case BEING_SHAVED:
            logClientMessage("is being shaved.");
            if(sharedData->clientToShave.clientPID==getpid() && sharedData->clientToShave.isShaved==YES){
                i++;
                clientState=LEAVING;
                sharedData->currentBarberState=CHECKS;
            }
            break;
        case CHECKING:
            logClientMessage("is checking barber.");

            if(sharedData->currentBarberState==SLEEPING){
                logClientMessage("is waking up barber.");
                sharedData->currentBarberState=INVITING;
                clientState=WAITING;
                sharedData->clientToShave.clientPID=getpid();
                sharedData->currentClientQuantity++;
            }else{
                logClientMessage("is checking queue.");

                if(sharedData->currentClientQuantity<sharedData->maxNumberOfClients){
                    logClientMessage("is joining end of the queue.");
                    clientState=WAITING;
                    joinQueue();
                }else{
                    logClientMessage("is leaving because there is no place in the queue.");
                    clientState=IDLE;
                }
            }
            break;
        case WAITING:
            if(sharedData->clientToShave.clientPID==getpid()){
                sharedData->clientToShave.isOnChair=YES;
                logClientMessage("is going to be shaved.");
                clientState=BEING_SHAVED;
            }else{
                logClientMessage("is waiting in the queue.");
            }
            break;
        case IDLE:
            logClientMessage("is outside.");
            clientState=CHECKING;
            break;
        case LEAVING:
            logClientMessage("is shaved and leaves the barber.");
            clientState=IDLE;
            break;
        default:
            break;
        }        
        releaseSemaphore(sharedData->semaphore);
    }

    logClientMessage("finishes its work");

    return 0;
}