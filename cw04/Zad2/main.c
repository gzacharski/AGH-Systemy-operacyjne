#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t parentPID = 0;
pid_t childPID = 0;

int signalsReceived = 0;
int signalsSent = 0;

int signal1, signal2;
int L, type, breakLoop;

void printFarewellMessage()
{
    if (childPID)
    {
        printf("%d singals sent from %s PID %d.\n",
               signalsSent,
               "parent",
               getpid());
    }

    printf("%d singals received by %s PID: %d.\n",
           signalsReceived,
           (childPID) ? "parent" : "child",
           getpid());
}

void sendSignal(pid_t pid, int sig, int response)
{
    /*if (response)
    {
        printf("PID:\t%d\t %s %d %s \tPID:\t%d.\n",
               pid,
               "<--",
               sig,
               "<--",
               getpid());
    }
    else
    {
        printf("PID:\t%d\t %s %d %s \tPID:\t%d.\n",
               getpid(),
               "-->",
               sig,
               "-->",
               pid);
    }*/

    kill(pid, sig);
}

void handleSignal1(int sigNum)
{
    //printf("PID:\t%d\treceived signal %d.\n", getpid(), sigNum);

    signalsReceived++;

    if (getpid() != parentPID)
    {
        sendSignal(parentPID, signal1, 1);
        signalsSent++;
    }
}

void handleSignal2(int sigNum)
{
    //printf("PID:\t%d\treceived signal %d.\n", getpid(), sigNum);

    printFarewellMessage();
    exit(EXIT_SUCCESS);
}

void handleSIGINT(int sigNumber)
{
    if (getpid() == parentPID)
    {
        puts("\nOdebrano sygnal SIGINT.");
        breakLoop = 1;
    }
}

void parseArgumennts(int argc, char **argv)
{
    if (argc == 3)
    {
        L = atoi(argv[1]);
        type = atoi(argv[2]);

        if (type < 1 || type > 3 || L <= 0)
        {
            puts("Invalid arguments.\nType: ./main {+int} {1|2|3}");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        puts("Invalid number of arguments.\nType: ./main {+int} {1|2|3}");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{

    parseArgumennts(argc, argv);

    signal1 = (type != 3) ? SIGUSR1 : SIGRTMIN + 1;
    signal2 = (type != 3) ? SIGUSR2 : SIGRTMIN + 2;

    struct sigaction action1;
    action1.sa_handler = handleSignal1;
    sigfillset(&action1.sa_mask);
    action1.sa_flags = 0;
    sigaction(signal1, &action1, NULL);

    struct sigaction action2;
    action2.sa_handler = handleSignal2;
    sigfillset(&action2.sa_mask);
    action2.sa_flags = 0;
    sigaction(signal2, &action2, NULL);

    signal(SIGINT, handleSIGINT);

    parentPID = getpid();
    childPID = fork();

    if (childPID < 0)
    {
        perror("Failed to fork.");
        exit(EXIT_FAILURE);
    }
    else if (childPID == 0)
    {
        //child process is waiting only for signals;
        while (1)
        {
            pause();
        }
    }
    else
    {
        //parent process
        for (int i = 0; i < L; i++)
        {
            if (breakLoop)
            {
                //process interrupted by SIGINT and finish its work
                break;
            }

            sendSignal(childPID, signal1, 0);
            signalsSent++;

            if (type == 2)
            {
                pause();
            }
        }
        sendSignal(childPID, signal2, 0);
        wait(0);
    }

    printFarewellMessage();

    return 0;
}