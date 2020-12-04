#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int isPaused=0;

void handleSIGINT(int sigNumber){
    puts("\nOdebrano sygnal SIGINT.");
    exit(EXIT_SUCCESS);
}

void handleSIGTSTP(int sig_no){

    isPaused=isPaused?0:1;
    if(isPaused){
        puts("\nOczekuje na:");
        puts("\tCTRL+Z - kontynuacja");
        puts("\tCTRL+C - zakonczenie programu.");
    }
}

int main(){

    signal(SIGINT,handleSIGINT);

    struct sigaction act;
    act.sa_handler=handleSIGTSTP;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    sigaction(SIGTSTP, &act, NULL);
    
    time_t rawtime;
    struct tm *currentTime;

    while(1){

        if(isPaused) {
            pause();
            puts(""); //console will start printing in new line
        }
        
        rawtime=time(NULL);

        if(rawtime==-1){
            puts("time() function failed.");
            exit(EXIT_FAILURE);
        }

        currentTime=localtime(&rawtime);

        if(currentTime==NULL){
            puts("localtime() function failed.");
            exit(EXIT_FAILURE);
        }

        printf("Current time is %02d:%02d:%02d\n", 
            currentTime->tm_hour,
            currentTime->tm_min,
            currentTime->tm_sec
        );

        sleep(1);
    }
    
    return 0;
}