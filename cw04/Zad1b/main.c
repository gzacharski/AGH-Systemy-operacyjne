#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

pid_t childpid=-1;
int isPaused=0;


void makefork(){
    childpid=fork();

    if(childpid<0){
        perror("Nie powolano procesu potomnego.");
        exit(EXIT_FAILURE);
    }else if(childpid==0){
        //child process
        execlp("bash","bash","date.sh",NULL);
    }else{
        //parent process
        printf("\nPID:%d\n",(int)getpid());
    }
}

void handleSIGINT(int sigNumber){
    puts("\nOdebrano sygnal SIGINT.");
    kill(childpid,SIGKILL);
    exit(EXIT_SUCCESS);
}

void handleSIGTSTP(int sig_no){

    isPaused=isPaused?0:1;
    if(isPaused){
        kill(childpid,SIGKILL);
        puts("\nOczekuje na:");
        puts("\tCTRL+Z - kontynuacja");
        puts("\tCTRL+C - zakonczenie programu.");
    }else{
        makefork();
    }
}

int main(){

    signal(SIGINT,handleSIGINT);

    struct sigaction act;
    act.sa_handler=handleSIGTSTP;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    sigaction(SIGTSTP, &act, NULL);

    makefork();

    while(1){

        if(isPaused) {
            pause();
            puts(""); //console will start printing in new line
        }
    }
    
    return 0;
}