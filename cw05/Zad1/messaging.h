#ifndef __MESSAGING_H__
#define __MESSAGING_H__

#include <sys/msg.h>
#include <signal.h>

//Client sends command DISCOVER to server in order to register itself.
#define DISCOVER 1

//Server sends command ACKNOWLEDGE to client in order to inform successful client's registration.
#define ACKNOWLEDGE 2

//If the server receives command END from a client, then sends back to the client current date and time as char array. 
//Once a client receives command END, then prints it to STDOUT.
#define TIME 3

//If the server receives command END from a client, then finishes its work once there is no messages in the message queue.
#define END 4

//Globally defined path to generate key value with ftok()
#define PATH "HOME"

#define PROJECT_ID 0x099

#define MAX_NUMBER_OF_CLIENTS 10

//define message structure
struct message{
    long mtype; 
    pid_t processId;
    char mtext[32];
};

char* convert(int cmd){

    char* text;

    switch (cmd)
    {
    case 1:
        text="DISCOVER";
        break;
    case 2:
        text="ACKNOWLEDGE";
        break;
    case 3:
        text="TIME";
        break;
    case 4:
        text="END";
        break;
    default:
        text="error";
        break;
    }

    return text;
}

#endif