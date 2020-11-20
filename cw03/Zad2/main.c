#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char** arguments;

struct Command { 
    char **arguments;
    int numberOfArguments;
};

struct Command parseLine(char *line){
    //function splits the string into words and return as structure

    char delimiter[]=" ";

    char *ptr=strtok(line,delimiter);
    arguments=calloc(sizeof(line)+1,sizeof(char*));

    int i=0;

    while (ptr!=NULL)
    {
        arguments[i]=malloc(sizeof(ptr));
        arguments[i]=ptr;
        i++;
        ptr=strtok(NULL,delimiter);
    }
    
    arguments[i-1][strlen(arguments[i-1])-1]='\0';
    arguments[i]=NULL;
    i++;
    
    struct Command task;
    task.arguments=arguments;
    task.numberOfArguments=i;

    free(ptr);

    return task;
}

int main(int argc, char *argv[]){

    FILE *file=fopen(argv[1],"r");

    if(file==NULL){
        perror("Unable to open a file!");
        exit(1);
    }

    //Read lines using POSIX function getline
    //This code won't work on Windows OS.
    char *line=NULL;
    size_t len=0;
    printf("PID of main program: \t%d\n", (int)getpid());
    
    while (getline(&line,&len,file)!=-1)
    {    
        if(strlen(line)==1){
            //line is empty
            continue;
        }

        printf("\nLine: %s",line);
        struct Command task=parseLine(line);

        pid_t cpid=fork();

        if(cpid<0){
            //error

            perror("Can't fork, something went wrong");
            exit(EXIT_FAILURE);

        }else if(cpid==0){
            //child process
            printf("PID of a task: \t\t%d\n", (int)getpid());
            
            
            int status=execvp(task.arguments[0],task.arguments);
            
            //if error
            if(status<0){
                printf("PID:%d. Invalid command. Child process stopped working.\n", (int)getpid());
            }

            free(line);
            free(arguments);

            fclose(file);

            exit(EXIT_FAILURE);

        }else{
            //parent process
            //pid_t w=waitpid(cpid,NULL,0);
            int status;
            wait(&status);

            if(status != 0){
				printf("PID:%d. Invalid command: %s. Program stopped working.\n",(int)getpid(), task.arguments[0]);
				
                free(line);
                free(arguments);

                fclose(file);
                exit(EXIT_FAILURE);
			}
        }
    }
    
    free(line);
    free(arguments);

    fclose(file);
    
    return 0;
}