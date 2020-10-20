#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "mycopy.h"


void copyBySystemFunction(char *fileSource, char *fileTarget, unsigned int recordQuantity, unsigned int recordLength){

    int source=open(fileSource,O_RDONLY);
    if(source<0){
        fprintf(stderr,"Can't open source file: %s\n", strerror(errno));
        exit(-1);
    }
    
    int target=open(fileTarget,O_WRONLY | O_CREAT, S_IRUSR|S_IWUSR);
    if(target<0){
        fprintf(stderr,"Can't open target file: %s\n",strerror(errno));
        exit(-1);
    }

    char *bufor=malloc(sizeof(unsigned char)*recordLength);

    for(int i=0; i<recordQuantity; i++){

        if(read(source,bufor,sizeof(unsigned char)*recordLength)<0){
            fprintf(stderr,"Can't read from source file: %s\n",strerror(errno));
            exit(-1);
        }

        if(write(target,bufor,sizeof(unsigned char)*recordLength)<0){
            fprintf(stderr,"Can't write to target file: %s\n",strerror(errno));
            exit(-1);
        }
    }

    close(source);
    close(target);

    free(bufor);
};

void copyByLibraryFunction(char *fileSource, char *fileTarget, unsigned int recordQuantity, unsigned int recordLength){

    FILE *source=fopen(fileSource,"r");
    if(source==NULL){
        fprintf(stderr,"Can't open source file: %s\n",strerror(errno));
        exit(-1);
    }
    
    FILE *target=fopen(fileTarget,"w");
    if(target==NULL){
        fprintf(stderr,"Can't open target file: %s\n",strerror(errno));
        exit(-1);
    }

    char *bufor=malloc(sizeof(unsigned char)*recordLength);

    for(int i=0; i<recordQuantity; i++){
        if(fread(bufor,sizeof(unsigned char),recordLength,source)!=recordLength){
            fprintf(stderr,"Can't read from source file: %s\n",strerror(errno));
            exit(-1);
        }
        
        if(fwrite(bufor,sizeof(unsigned char), recordLength, target)!=recordLength){
            fprintf(stderr,"Can't write to target file: %s\n",strerror(errno));
            exit(-1);
        }
    }

    fclose(source);
    fclose(target);

    free(bufor);
};