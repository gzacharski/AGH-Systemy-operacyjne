#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <fcntl.h>
#include <time.h>
#include "messaging.h"

mqd_t clientQueueDs;
mqd_t serverQueueDs;
int activeClients=0;
mqd_t clientQueueDss[MAX_NUMBER_OF_CLIENTS];

void closeClientQueueAtExit()
{   
    puts("Closing server message queue...");
    if(mq_close(serverQueueDs)==-1){
        perror("Can't close clinent queue.");
        exit(EXIT_FAILURE);
    }

    for(int i=0; i<activeClients;i++){
        puts("Closing client message queue...");
        if(mq_close(clientQueueDss[i])==-1){
            perror("Can't close client message queue.");
            exit(EXIT_FAILURE);
        }
    }

    puts("Removing server message queue...");
    if(mq_unlink(SERVER_NAME)==-1){
        perror("Can't delete client queue.");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{

    if (atexit(closeClientQueueAtExit) == -1)
    {
        perror("Can't create atexit function for client\n");
        exit(EXIT_FAILURE);
    }

    //define server message queue configuration
    struct mq_attr serverQueueConfig;
    serverQueueConfig.mq_maxmsg=10;
    serverQueueConfig.mq_msgsize=sizeof(struct message);
    
    //create server message queue
    serverQueueDs=mq_open(SERVER_NAME,O_RDWR | O_CREAT | O_EXCL, 0666, &serverQueueConfig );
    if(serverQueueDs==-1){
        perror("Can't open server message queue. Server is unavailable.\n");
        exit(EXIT_FAILURE);
    }

    struct message msg;
    time_t rawtime;
    struct tm *timeinfo;
    int finishWork=0;

    while (1)
    {   
        if(mq_getattr(serverQueueDs,&serverQueueConfig)==-1){
            printf("Can't read");
            exit(EXIT_FAILURE);
        } 
    
        if(serverQueueConfig.mq_curmsgs==0){
            if(finishWork){
                //finish server work
                break;
            }
            continue;
        }

        if((mq_receive(serverQueueDs,(char*) &msg,sizeof(struct message),NULL))!=-1){

            printf("Message %s received from PID: %d\n",convert(msg.mtype), msg.processId);
            
            if(msg.mtype==DISCOVER){
                //receive DISCOVER command

                activeClients++;
                if(activeClients>MAX_NUMBER_OF_CLIENTS){
                    printf("Maximum number of clients. Server will be closed.");
                    exit(EXIT_FAILURE);
                }

                //register client on server
                clientQueueDs=mq_open(msg.mtext,O_RDWR);
                if(clientQueueDs==-1){
                    printf("Can't open client message queues.");
                    exit(EXIT_FAILURE);
                }

                clientQueueDss[activeClients-1]=clientQueueDs;
                
                //prepare message for client
                msg.mtype = ACKNOWLEDGE;
                msg.processId=getpid();
                sprintf(msg.mtext, "%d", clientQueueDs);
                
            }else if(msg.mtype==TIME){
                //receive TIME command
                sscanf(msg.mtext,"%d",&clientQueueDs);
                
                int tempClientQueueDs=-1;

                //find client id by its key
                for (int i = 0; i < MAX_NUMBER_OF_CLIENTS; i++){
                    if (clientQueueDs == clientQueueDss[i]){
                        tempClientQueueDs = clientQueueDss[i];
                        break;
                    }
                }

                if(tempClientQueueDs==-1){
                    printf("Can't find client\n");
                    exit(EXIT_FAILURE);
                }

                //prepare message for client
                time(&rawtime);
                timeinfo = localtime(&rawtime);

                msg.mtype = TIME;
                msg.processId=getpid();
                strcpy(msg.mtext, asctime(timeinfo));

            }else if(msg.mtype==END){
                //receive END command
                //receive another command and finish work
                finishWork=1;
                
            }else{
                return -1;
            }

            //send response to client
            if(mq_send(clientQueueDs,(char*) &msg,sizeof(struct message),1)==-1){
                perror("Can't send message to the client queue.");
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}