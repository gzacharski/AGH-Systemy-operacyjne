#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include "messaging.h"

int serverMessageQueueId=-1;

void closeClientQueueAtExit()
{
    puts("Removing message queue...");

    if (serverMessageQueueId != -1)
    {
        if (msgctl(serverMessageQueueId, IPC_RMID, NULL) == -1)
        {
            printf("Cant't delete message queue");
            exit(EXIT_FAILURE);
        }
        serverMessageQueueId = -1;
    }
}

int main(int argc, char *argv[])
{

    if (atexit(closeClientQueueAtExit) == -1)
    {
        printf("Can't create atexit function for client\n");
        exit(EXIT_FAILURE);
    }

    int clientIDs[MAX_NUMBER_OF_CLIENTS];
    key_t clientKeys[MAX_NUMBER_OF_CLIENTS];

    int clientMessageQueueId=-1;
    int activeClients=0;
    int finishWork=0;

    char *path = getenv(PATH);
    if (path == NULL)
    {
        printf("Can't find path\n");
        exit(EXIT_FAILURE);
    }

    //open server message queue
    key_t serverKey = ftok(path, PROJECT_ID);
    if ((serverMessageQueueId = msgget(serverKey, IPC_CREAT | 0666)) == -1)
    {
        perror("Can't open server message queue.");
        exit(EXIT_FAILURE);
    }

    struct message msgBuf;
    time_t rawtime;
    struct tm *timeinfo;
    int messageSize = sizeof(struct message) - sizeof(long);

    while (1)
    {   
        if(msgrcv(serverMessageQueueId, &msgBuf, messageSize, 0, IPC_NOWAIT)!=-1){

            printf("Message %s received from PID: %d\n",convert(msgBuf.mtype), msgBuf.processId);
            
            if(msgBuf.mtype==DISCOVER){
                //receive DISCOVER command

                activeClients++;
                if(activeClients>MAX_NUMBER_OF_CLIENTS){
                    printf("Maximum number of clients. Server will be closed.");
                    exit(EXIT_FAILURE);
                }

                //register client on server
                key_t clientKey;
                sscanf(msgBuf.mtext,"%d",&clientKey);

                int clientId=msgget(clientKey,0);
                if(clientId==-1){
                    printf("Can't open client message queues.");
                    exit(EXIT_FAILURE);
                }

                clientIDs[activeClients-1]=clientId;
                clientKeys[activeClients-1]=clientKey;

                clientMessageQueueId=clientId;
                
                //prepare message for client
                msgBuf.mtype = ACKNOWLEDGE;
                msgBuf.processId=getpid();
                sprintf(msgBuf.mtext, "%d", clientId);
                
            }else if(msgBuf.mtype==TIME){
                //receive TIME command
                key_t clientKey;
                sscanf(msgBuf.mtext,"%d",&clientKey);
                
                clientMessageQueueId=-1;

                //find client id by its key
                for (int i = 0; i < MAX_NUMBER_OF_CLIENTS; i++){
                    if (clientKey == clientKeys[i]){
                        clientMessageQueueId = clientIDs[i];
                        break;
                    }
                }

                if(clientMessageQueueId==-1){
                    printf("Can't find client\n");
                    exit(EXIT_FAILURE);
                }
                
                //send TIME response
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                
                //prepare message for client
                msgBuf.mtype = TIME;
                msgBuf.processId=getpid();
                strcpy(msgBuf.mtext, asctime(timeinfo));

            }else if(msgBuf.mtype==END){
                //receive END command
                //receive another command and finish work
                finishWork=1;
                
            }else{
                return -1;
            }

            if(msgsnd(clientMessageQueueId,&msgBuf,messageSize,0)==-1)
            {
                perror("Can't send message to the queue.");
                exit(EXIT_FAILURE);
            }

        }else{
            if(finishWork){
                //finish server work
                break;
            }
        }
    }

    return 0;
}