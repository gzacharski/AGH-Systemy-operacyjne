#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char *command[32][32];
int n_cmd; //number of commands in current line;

void parseLine(char *line)
{
    //function parse line with commands
    int i = 0;                     //row of command 2D array
    int j = 0;                     //column of command 2D array
    char *ptr = strtok(line, " "); //spliting string

    while (ptr != NULL)
    {
        if (ptr[0] == '|' && ptr[1] == '\0')
        {
            command[i][j] = NULL;
            j = 0;
            i++;
        }
        else
        {
            command[i][j] = ptr;
            j++;
        }
        ptr = strtok(NULL, " \n");
    }

    n_cmd = i;
}

int main(int argc, char *argv[])
{

    FILE *file = fopen(argv[1], "r");

    if (file == NULL)
    {
        perror("Unable to open a file!");
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, file) != -1)
    {
        if (strlen(line) == 1)
        {
            //line is empty
            continue;
        }

        if (strlen(line) > 3 && line[0] == '/' && line[1] == '/')
        {
            //line is a comment
            continue;
        }

        parseLine(line);

        int fd[n_cmd][2];
        pid_t pid[n_cmd];

        for (int i = 0; i <= n_cmd; i++)
        {
            if(pipe(fd[i])<0){
                perror("Can't create pipe");
                exit(EXIT_FAILURE);
            }

            pid[i] = fork();

            if (pid[i] < 0)
            {
                perror("Can't make fork");
                exit(EXIT_FAILURE);
            }
            else if (pid[i] == 0)
            {
                //child process

                if (i > 0)
                {   
                    close(fd[i-1][1]);
                    dup2(fd[i-1][0],STDIN_FILENO);
                    close(fd[i-1][0]);
                }

                if (i < n_cmd)
                {
                    close(fd[i][0]);
                    dup2(fd[i][1],STDOUT_FILENO);
                    close(fd[i][1]);
                }

                if(execvp(command[i][0], command[i])<0){
                    perror("Can't execute command :(");
                    exit(EXIT_FAILURE);
                }
            }
        }

        //parent process
        for (int i = 0; i < n_cmd; i++)
        {
            waitpid(pid[i], NULL, 0);
        }

    }

    fclose(file);

    return 0;
}