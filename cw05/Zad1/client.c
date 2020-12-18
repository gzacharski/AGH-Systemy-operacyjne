#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include "messaging.h"

int clientMessageQueueId = -1;

void handleSIGINT(int sigNumber)
{
    puts("Signal received.Finish working...");
    exit(EXIT_SUCCESS);
}

void closeClientQueueAtExit()
{
    puts("Removing message queue...");

    if (clientMessageQueueId != -1)
    {
        if (msgctl(clientMessageQueueId, IPC_RMID, NULL) == -1)
        {
            printf("Cant't delete message queue\n");
            exit(EXIT_FAILURE);
        }
        clientMessageQueueId = -1;
    }
}

int main(int argc, char *argv[])
{
    int serverMessageQueueId;
    int messageSize = sizeof(struct message) - sizeof(long);

    if (atexit(closeClientQueueAtExit) == -1)
    {
        puts("Can't create atexit function for client\n");
        exit(EXIT_FAILURE);
    }

    //handle iterruption signal
    signal(SIGINT, handleSIGINT);

    char *path = getenv(PATH);
    if (path == NULL)
    {
        puts("Can't find path\n");
        exit(EXIT_FAILURE);
    }

    //create client's unique message queue
    key_t clientKey = ftok(path, getpid());
    clientMessageQueueId = msgget(clientKey, IPC_CREAT | IPC_EXCL | 0666);
    if (clientMessageQueueId == -1)
    {
        perror("Can't create new message queue.");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\tKey: %x\tID: %d\n", getpid(), clientKey, clientMessageQueueId);

    //open server message queue
    key_t serverKey = ftok(path, PROJECT_ID);
    if ((serverMessageQueueId = msgget(serverKey, 0)) == -1)
    {
        puts("Can't open server message queue. Server is unavailable.");
        exit(EXIT_FAILURE);
    }

    printf("Server: \tKey: %x\tID: %d\n", serverKey,serverMessageQueueId);

    //create message to register on server
    struct message msgBuf;
    msgBuf.mtype = DISCOVER;
    msgBuf.processId = getpid();
    sprintf(msgBuf.mtext, "%d", clientKey);

    //register itselft on server
    if(msgsnd(serverMessageQueueId,&msgBuf,messageSize,0)==-1)
    {
        perror("Can't send message to the queue.");
        exit(EXIT_FAILURE);
    }

    //wait for information about successful registration
    if (msgrcv(clientMessageQueueId, &msgBuf, messageSize, ACKNOWLEDGE, 0) == -1)
    {
        perror("Can't receive message from the queue");
        return -1;
    }

    FILE* file=NULL;

    if(argc==2){
        file=fopen(argv[1],"r");
    }
    
    while (1)
    {
        sleep(1);
        char cmd[5];

        if(file==NULL){
            //user types a command
            printf("PID: %d - Type command: TIME or END\n",getpid());
            scanf("%s",cmd);
        }else{
            if(fgets(cmd,5,file)==NULL){
                exit(EXIT_FAILURE);
            }
        }

        //compare a command
        if(strcmp("TIME",cmd)==0){
            msgBuf.mtype = TIME;
        }else if(strcmp("END",cmd)==0){
            msgBuf.mtype = END;
        }else{
            continue;
        }

        //prepare message
        msgBuf.processId = getpid();
        sprintf(msgBuf.mtext, "%d", clientKey);

        //send message 
        if (msgsnd(serverMessageQueueId, &msgBuf, messageSize, 0) == -1)
        {
            perror("Can't send message to the queue.");
            exit(EXIT_FAILURE);
        }

        if (msgBuf.mtype == TIME)
        {   
            //wait for response
            if (msgrcv(clientMessageQueueId, &msgBuf, messageSize, 0, 0) == -1){
                printf("Can't receive message.\n");
                exit(EXIT_FAILURE);
            }
            printf("Time: %s", msgBuf.mtext);
        }
        else
        {
            //command END was sent, program finishes its work....
            puts("Finish working...");
            return 0;
        }
    }

    return 0;
}