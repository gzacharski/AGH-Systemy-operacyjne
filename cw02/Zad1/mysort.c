#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "mysort.h"
#include "mylog.h"

void sortBySystemFunction(char *fileName, unsigned int recordQuantity, unsigned int recordLength){
    
    int file=open(fileName,O_RDWR);
    if(file<0){
        fprintf(stderr,"Can't open file: %s\n", strerror(errno));
        exit(-1);
    }

    if(file){
        unsigned char *bufor1=malloc(sizeof(unsigned char)*recordLength);
        unsigned char *bufor2=malloc(sizeof(unsigned char)*recordLength);

        for(int i=1; i<recordQuantity; i++){
            if(lseek(file,(i-1)*sizeof(unsigned char)*recordLength,SEEK_SET)<0){
                fprintf(stderr,"Can't seek in file: %s\n",strerror(errno));
                exit(-1);
            }
            if(read(file,bufor1,sizeof(unsigned char)*recordLength)<0){
                fprintf(stderr,"Can't read from file: %s\n",strerror(errno));
                exit(-1);
            }
            if(read(file,bufor2,sizeof(unsigned char)*recordLength)<0){
                fprintf(stderr,"Can't read from file: %s\n",strerror(errno));
                exit(-1);
            }

            int j=i;
            while(j>0 && bufor1[0]>bufor2[0]){
                if(lseek(file,j*sizeof(unsigned char)*recordLength,SEEK_SET)<0){
                    fprintf(stderr,"Can't seek in file: %s\n",strerror(errno));
                    exit(-1);
                }
                if(write(file,bufor1,sizeof(unsigned char)*recordLength)<0){
                    fprintf(stderr,"Can't write into file: %s\n",strerror(errno));
                    exit(-1);
                }

                j--;

                if(j>0){
                    if(lseek(file,(j-1)*sizeof(unsigned char)*recordLength,SEEK_SET)<0){
                        fprintf(stderr,"Can't seek in file: %s\n",strerror(errno));
                        exit(-1);
                    }
                    if(read(file,bufor1,sizeof(unsigned char)*recordLength)<0){
                        fprintf(stderr,"Can't read from file: %s\n",strerror(errno));
                        exit(-1);
                    }
                }
            }

            if(lseek(file,j*sizeof(unsigned char)*recordLength,SEEK_SET)<0){
                fprintf(stderr,"Can't seek in file: %s\n",strerror(errno));
                exit(-1);
            }

            if(write(file,bufor2,sizeof(unsigned char)*recordLength)<0){
                fprintf(stderr,"Can't write into file: %s\n",strerror(errno));
                exit(-1);
            }
        }

        free(bufor1);
        free(bufor2);
        close(file);
    }
};

void sortByLibraryFunction(char *fileName, unsigned int recordQuantity, unsigned int recordLength){
    
    FILE *file=fopen(fileName, "r+");
    if(file==NULL){
        fprintf(stderr,"Can't open file: %s\n",strerror(errno));
        exit(-1);
    }
    
    if(file){
        unsigned char *bufor1=malloc(sizeof(unsigned char)*recordLength);
        unsigned char *bufor2=malloc(sizeof(unsigned char)*recordLength);

        for(int i=1; i<recordQuantity; i++){

            if(fseek(file,(i-1)*sizeof(unsigned char)*recordLength,0)!=0){
                fprintf(stderr,"Can't seek in file: %s\n",strerror(errno));
                exit(-1);
            }
        
            if(fread(bufor1,sizeof(unsigned char),recordLength,file)!=recordLength){
                fprintf(stderr,"Can't read from file: %s\n",strerror(errno));
                exit(-1);
            }
            if(fread(bufor2,sizeof(unsigned char),recordLength,file)!=recordLength){
                fprintf(stderr,"Can't read from file: %s\n",strerror(errno));
                exit(-1);
            }

            int j=i;
            while(j>0 && bufor1[0]>bufor2[0]){

                if(fseek(file,j*sizeof(unsigned char)*recordLength,0)!=0){
                    fprintf(stderr,"Can't seek in file: %s\n",strerror(errno));
                    exit(-1);
                }

                if(fwrite(bufor1,sizeof(unsigned char),recordLength,file)!=recordLength){
                    fprintf(stderr,"Can't write into file: %s\n",strerror(errno));
                    exit(-1);
                }

                j--;

                if(j>0){
                    if(fseek(file,(j-1)*sizeof(unsigned char)*recordLength,0)!=0){
                        fprintf(stderr,"Can't seek in file: %s\n",strerror(errno));
                        exit(-1);
                    }

                    if(fread(bufor1,sizeof(unsigned char),recordLength,file)!=recordLength){
                        fprintf(stderr,"Can't read from file: %s\n",strerror(errno));
                        exit(-1);
                    }
                }
            }

            if(fseek(file,j*sizeof(unsigned char)*recordLength,0)!=0){
                fprintf(stderr,"Can't seek in file: %s\n",strerror(errno));
                exit(-1);
            }
            if(fwrite(bufor2,sizeof(unsigned char),recordLength,file)!=recordLength){
                fprintf(stderr,"Can't write into file: %s\n",strerror(errno));
                exit(-1);
            }
        }

        free(bufor1);
        free(bufor2);
        fclose(file);
    }
};