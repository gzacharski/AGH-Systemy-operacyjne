#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "mycopy.h"
#include "mygenerate.h"
#include "mylog.h"
#include "mysort.h"

void logDuration(struct tms *st_cpu, struct tms *en_cpu, clock_t st_time, clock_t en_time){
    int tics_per_second=sysconf(_SC_CLK_TCK);

    double realTime=((double)(en_time-st_time))/tics_per_second;
    double userTime=((double)(en_cpu->tms_utime - st_cpu->tms_utime))/tics_per_second;
    double systemTime=((double)(en_cpu->tms_stime - st_cpu->tms_stime))/tics_per_second;

    printf("Real time:\t%15fs\n",realTime);
    printf("User time:\t%15fs\n",userTime);
    printf("System time:\t%15fs\n",systemTime);
}

int main(int argc, char* argv[]){

    clock_t st_time, en_time;

    struct tms *st_cpu=malloc(sizeof(struct tms));
    struct tms *en_cpu=malloc(sizeof(struct tms));

    if(argc<5) {
        logError();
        return 0;
    }

    logInfo(argc, argv);

    char *command=argv[1];
    char *fileName=argv[2];
    char *fileCopied=(argc==7)?argv[3]:NULL;
    int recordQuantity=atoi((argc==7)?argv[4]:argv[3]);
    int recordLength=atoi((argc==7)?argv[5]:argv[4]);

    st_time=times(st_cpu);

    switch (argc){
        case 5:
            if(strcmp(command,"generate")==0){
                 generate(fileName,recordQuantity,recordLength);
            }else if(strcmp(command,"read")==0){
                readFile(fileName,recordQuantity,recordLength);
            }else{
                logError();
            }
            break;
        case 6:
            if(strcmp(command,"sort")==0){

                if(strcmp(argv[5],"sys")==0){
                    sortBySystemFunction(fileName,recordQuantity,recordLength);
                }else if(strcmp(argv[5],"lib")==0){
                    sortByLibraryFunction(fileName,recordQuantity,recordLength);
                }else{
                    logError();
                }
            }else{
                logError();
            }
            break;
        case 7:
            if(strcmp(command,"copy")==0){

                if(strcmp(argv[6],"sys")==0){
                    copyBySystemFunction(fileName,fileCopied,recordQuantity,recordLength);
                }else if(strcmp(argv[6],"lib")==0){
                    copyByLibraryFunction(fileName,fileCopied,recordQuantity,recordLength);
                }else{
                    logError();
                }
            }else{
                logError();
            }  
            break;
        default:
            logError();
            break;
    }

    en_time=times(en_cpu);

    logDuration(st_cpu,en_cpu,st_time,en_time);

    free(st_cpu);
    free(en_cpu);
    printf("\n\n");
    return 0;
}