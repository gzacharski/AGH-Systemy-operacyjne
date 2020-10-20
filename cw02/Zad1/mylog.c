#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include "mylog.h"

void logInfo(int argc, char* argv[]){
    printf("----------------\n");
    printf("Command:\t\t%s\n", argv[1]);

    if(argc<7) {
        printf("Name of file:\t\t%s\n", argv[2]);
    }else{
        printf("Name of files:\t\t%s -> %s\n", argv[2], argv[3]);  
    }

    printf("Quantity of record:\t%d\n", atoi((argc==7)?argv[4]:argv[3]));
    printf("Length of record:\t%d\n", atoi((argc==7)?argv[5]:argv[4]));
    if(argc>5) printf("Mode:\t\t\t%s\n", (argc==6)?argv[5]:argv[6]);
    printf("----------------\n");
}

void logError(){
    printf("Error occured. Type: \n");
    printf("program generate fileName recordQuantity recordLength\n");
    printf("program sort fileName recordQuantity recordLength sys|lib\n");
    printf("program copy fileSource fileTarget recordQuantity recordLenght sys|lib\n");
}
