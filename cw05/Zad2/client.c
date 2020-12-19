#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <fcntl.h>
#include "messaging.h"

mqd_t clientQueueDs;
mqd_t serverQueueDs;
char clientQueueName[16];

void handleSIGINT(int sigNumber)
{
    puts("\nSignal received.Finish working...");
    exit(EXIT_SUCCESS);
}

void closeClientQueueAtExit()
{   
    puts("Closing client message queue...");
    if(mq_close(clientQueueDs)==-1){
        perror("Can't close client message queue.");
        exit(EXIT_FAILURE);
    }

    puts("Closing server message queue...");
    if(mq_close(serverQueueDs)==-1){
        perror("Can't close server message queue.");
        exit(EXIT_FAILURE);
    }

    puts("Removing client message queue...");
    if(mq_unlink(clientQueueName)==-1){
        perror("Can't delete client message queue.");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    if (atexit(closeClientQueueAtExit) == -1)
    {
        puts("Can't create atexit function for client\n");
        exit(EXIT_FAILURE);
    }

    //handle iterruption signal
    signal(SIGINT, handleSIGINT);

    //create random client queue name
    sprintf(clientQueueName,"/%d",getpid());

    struct mq_attr clientQueueConfig;
    clientQueueConfig.mq_maxmsg=10;
    clientQueueConfig.mq_msgsize=sizeof(struct message);
    
    //create client's unique message queue
    clientQueueDs=mq_open(clientQueueName,O_RDWR | O_CREAT | O_EXCL, 0666,&clientQueueConfig );
    if(clientQueueDs==-1){
        perror("Can't create client queue.\n");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\t\n", getpid());

    //open server message queue
    serverQueueDs=mq_open(SERVER_NAME,O_RDWR );
    if(serverQueueDs==-1){
        printf("Can't open server message queue. Server is unavailable.\n");
        exit(EXIT_FAILURE);
    }
    
    //create message to register on server
    struct message msg;
    msg.mtype = DISCOVER;
    msg.processId = getpid();
    strcpy(msg.mtext,clientQueueName);

    //send message to serve in order to register 
    if(mq_send(serverQueueDs,(char*) &msg,sizeof(struct message),1)==-1){
        perror("Can't send message to the server queue.");
        exit(EXIT_FAILURE);
    }

    //receive acknowledge from server
    if(mq_receive(clientQueueDs,(char*) &msg,sizeof(struct message),NULL)==-1){
        perror("Can't receive message from the queue");
        return -1;
    }

    //server id/descriptor to client
    int idOnServer;
    sscanf(msg.mtext,"%d",&idOnServer);

    printf("Message %s received from PID: %d\n",convert(msg.mtype), msg.processId);

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
            msg.mtype = TIME;
        }else if(strcmp("END",cmd)==0){
            msg.mtype = END;
        }else{
            continue;
        }

        //prepare message
        msg.processId = getpid();
        sprintf(msg.mtext, "%d", idOnServer);

        //send message 
        if(mq_send(serverQueueDs,(char*) &msg,sizeof(struct message),1)==-1){
            perror("Can't send message to the server queue.");
            exit(EXIT_FAILURE);
        }

        if (msg.mtype == TIME)
        {   
            //wait for response
            if(mq_receive(clientQueueDs,(char*) &msg,sizeof(struct message),NULL)==-1){
                perror("Can't receive message from the queue");
                return -1;
            }
            printf("Time: %s", msg.mtext);
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