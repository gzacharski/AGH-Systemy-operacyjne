#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "mygenerate.h"

void generate(char *fileName, unsigned int recordQuantity, unsigned int recordLength){
    FILE *file=fopen(fileName, "w");
    if(file==NULL){
        fprintf(stderr,"Can't open file: %s\n",strerror(errno));
        exit(-1);
    }

    if(file){
        time_t t;
        srand((unsigned) time(&t));
        unsigned char *record=malloc(sizeof(unsigned char)*recordLength);
        
        for(int i=0; i<recordQuantity; i++){

            for(int j=0; j<recordLength; j++){
                record[j]=(unsigned char) random()%256;
                //printf("%d%s", record[j],(j+1==recordLength)?"\n":"\t");
            }

            if(fwrite(record,sizeof(unsigned char),recordLength,file)<0){
                fprintf(stderr,"Can't write to file: %s\n",strerror(errno));
                exit(-1);
            }
        }

        free(record);
        fclose(file);
    }
}

void readFile(char *fileName, unsigned int recordQuantity, unsigned int recordLength){

    FILE *file=fopen(fileName, "r");
     if(file==NULL){
        fprintf(stderr,"Can't read file: %s\n",strerror(errno));
        exit(-1);
    }

    if(file){
        unsigned char *record=malloc(sizeof(unsigned char)*recordLength);
        
        for(int i=0; i<recordQuantity; i++){
            if(fread(record,sizeof(unsigned char),recordLength,file)<0){
                fprintf(stderr,"Can't read from file: %s\n",strerror(errno));
                exit(-1);
            }

            for(int j=0; j<recordLength; j++){
                printf("%d%s", record[j],(j+1==recordLength)?"\n":"\t");
            }
        }

        free(record);
        fclose(file);
    }
}