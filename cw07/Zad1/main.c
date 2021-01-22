#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

int producersNumber;
int consumersNumber;
int bufferSize;
char* sourceFile;
int maxLength;
int searchMode;
int debugMode;
int nk;

FILE *sourceTextFile;
pthread_t* consumerThreads;
pthread_t* producerThreads;
char** buffer;
int indexToProduce=0;
int indexToConsume=0;

pthread_mutex_t* lockBuffer;
pthread_mutex_t lockProducer;
pthread_mutex_t lockConsumer;

pthread_cond_t canProduce;
pthread_cond_t canConsume;

void signalHandler(int sigNum){

    for(int i=0; i<consumersNumber; i++){
        pthread_cancel(consumerThreads[i]);
    }

    for(int i=0; i<producersNumber; i++){
        pthread_cancel(producerThreads[i]);
    }

    printf("\nCorrect interruption\n");

    exit(EXIT_SUCCESS);
}

int readConfigFile(char* fileName){

    FILE *configFile=fopen(fileName,"r");
    if(configFile==NULL){
        perror("Unable to open a file");
        exit(EXIT_FAILURE);
    }

    char *line=NULL;
    size_t length=0;

    while(getline(&line,&length,configFile)!=-1){
        if(strlen(line)==1 || strlen(line)==0){
            //line is empty
            continue;

        }else if(line[0]=='/' && line[1]=='/'){
            //comment
            continue;

        }else if(line[0]=='P' && line[1]=='='){
            producersNumber=atoi(&line[2]);

        }else if(line[0]=='K' && line[1]=='='){
            consumersNumber=atoi(&line[2]);

        }else if(line[0]=='N' && line[1]=='='){
            bufferSize=atoi(&line[2]);

        }else if(line[0]=='L' && line[1]=='='){
            maxLength=atoi(&line[2]);

        }else if(line[0]=='T' && line[2]=='='){
            searchMode=atoi(&line[3]);

        }else if(line[0]=='T' && line[3]=='='){
            debugMode=atoi(&line[4]);

        }else if(line[0]=='n' && line[2]=='='){
            nk=atoi(&line[3]);

        }else{
            sourceFile=malloc(sizeof(line));
            strcpy(sourceFile,line);
            sourceFile[strlen(sourceFile)-1]='\0';
        }
    }

    fclose(configFile);

    return 0;
}

void logParams(){
    printf("\nP=%d\n",producersNumber);
    printf("K=%d\n",consumersNumber);
    printf("N=%d\n",bufferSize);
    printf("S=%s\n",sourceFile);
    printf("L=%d\n",maxLength);
    printf("TW=%d\n",searchMode);
    printf("TWI=%d\n",debugMode);
    printf("nk=%d\n",nk);
}

void *producerThread(void * arg){

    struct timespec monotime;

    while(1){
       
        pthread_mutex_lock(&lockProducer);

        char *line=NULL;
        size_t length=0;

        if(debugMode) printf("Producer %ld gets line\n",pthread_self());

        if(getline(&line,&length,sourceTextFile)==-1){
            perror("All file has been read");
            exit(EXIT_FAILURE);
        }

        while(buffer[indexToProduce]!=NULL){
            pthread_cond_wait(&canProduce,&lockProducer);
        }

        pthread_mutex_lock(&lockBuffer[indexToProduce]);

        clock_gettime(CLOCK_MONOTONIC,&monotime);

        if(debugMode) {
            printf("Producer %ld takes buffer index: %d -time: %ld.%ld\n",
                pthread_self(),
                indexToProduce,
                monotime.tv_sec,
                monotime.tv_nsec
            );
        }

        buffer[indexToProduce]=malloc((strlen(line)+1)*sizeof(char));
        line[strlen(line)-1]='\0';
        strcpy(buffer[indexToProduce],line);
        
        if(debugMode) {
            printf("Producer %ld saves line into buffer\n",
                pthread_self()
            );
        }

        indexToProduce=(indexToProduce+1)%bufferSize;

        pthread_mutex_unlock(&lockBuffer[indexToProduce]);

        pthread_cond_broadcast(&canConsume);
        pthread_mutex_unlock(&lockProducer);

        if(debugMode) {
            printf("Producer %ld unlocks mutexes and broadcast to consumers\n",
                pthread_self()
            );
        }
    }

    return NULL;
}

void *consumerThread(void * arg){

    struct timespec monotime;

    while(1){
        pthread_mutex_lock(&lockConsumer);
        if(debugMode) printf("Consumer %ld gets a consumerMutex\n",pthread_self());

        //wait until producer produce sth into the buffer
        while(buffer[indexToConsume]==NULL){
            pthread_cond_wait(&canConsume,&lockConsumer);
        }

        pthread_mutex_lock(&lockBuffer[indexToConsume]);

        clock_gettime(CLOCK_MONOTONIC,&monotime);

        if(debugMode) {
            printf("Consumer %ld takes buffer index: %d -time: %ld.%ld\n",
                pthread_self(),
                indexToConsume,
                monotime.tv_sec,
                monotime.tv_nsec
            );
        }
            
        char *line=malloc((strlen(buffer[indexToConsume])+1)*sizeof(char));
        //read line
        strcpy(line,buffer[indexToConsume]);
        line[strlen(line)-1]='\0';

        //compare
        if(strlen(line)>maxLength){
            if(debugMode) {
                printf("Consumer %ld finds a proper line: time: %ld.%ld\n",
                    pthread_self(),
                    monotime.tv_sec,
                    monotime.tv_nsec
                );
            }
            printf("%s\n",line);
        }
        
        //clean buffer
        buffer[indexToConsume]=NULL;
        free(line);
        indexToConsume=(indexToConsume+1)%bufferSize;

        //unlock mutexes
        pthread_mutex_unlock(&lockBuffer[indexToConsume]);
        pthread_cond_broadcast(&canProduce);
        pthread_mutex_unlock(&lockConsumer);

        if(debugMode) {
            printf("Consumer %ld unlocks its mutexes and broadcast to writers\n",
                pthread_self()
            );
        }
        usleep(10);
    }

    return NULL;
}

void initializeThreads(){

    signal(SIGINT,signalHandler);

    sourceTextFile=fopen(sourceFile,"r");

    consumerThreads=calloc(consumersNumber,sizeof(pthread_t));
    producerThreads=calloc(producersNumber,sizeof(pthread_t));

    buffer=malloc(bufferSize*sizeof(char*));
    for(int i=0; i<bufferSize;i++){
        buffer[i]=NULL;
    }

    pthread_mutex_init(&lockConsumer,NULL);
    pthread_mutex_init(&lockProducer,NULL);

    lockBuffer=calloc(bufferSize,sizeof(pthread_mutex_t));

    for(int i=0; i<bufferSize;i++){
        pthread_mutex_init(&lockBuffer[i],NULL);
    }

    pthread_cond_init(&canConsume,NULL);
    pthread_cond_init(&canProduce,NULL);
}

void runThreads(){
    for(int i=0; i<consumersNumber; i++){
        pthread_create(&consumerThreads[i],NULL,consumerThread,NULL);
    }

    for(int i=0; i<producersNumber; i++){
        pthread_create(&producerThreads[i],NULL,producerThread,NULL);
    }

    if(nk>0) alarm(nk);
}

void joinThreads(){

    for(int i=0; i<producersNumber; i++){
        pthread_join(producerThreads[i],NULL);
    }

    for(int i=0; i<consumersNumber; i++){
        pthread_join(consumerThreads[i],NULL);
    }
}

void cleanUp(){

    fclose(sourceTextFile);
    free(consumerThreads);
    free(producerThreads);
    free(buffer);

    pthread_mutex_destroy(&lockConsumer);
    pthread_mutex_destroy(&lockProducer);

    for(int i=0; i<bufferSize;i++){
        pthread_mutex_destroy(&lockBuffer[i]);
    }

    pthread_cond_destroy(&canConsume);
    pthread_cond_destroy(&canProduce);
}


int main(int argc, char *argv[]){

    if(argc==2){
        readConfigFile(argv[1]);
    }else{
        printf("Type config file as parameter.\n");
        printf("%s [config file]\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    time_t t;
    srand((unsigned) time(&t));

    logParams();

    initializeThreads();

    runThreads();

    joinThreads();

    cleanUp();

    return 0;
}