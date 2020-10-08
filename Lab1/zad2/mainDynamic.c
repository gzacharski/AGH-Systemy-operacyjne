//Zadanie 2 
//Program wypisuje liczby doskonale w zakresie NUMBER.
//Liczba doskonala to taka liczba, ktorej suma dzielnikow, oprocz niej samej,
//jest rowna rozpatrywanej liczbie.

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L
#define NUMBER 100000
#define SIZE 4

int task();
double getDuration(struct timespec *, struct timespec *);
void printNumbers(int *);

int main(){

    struct timespec startTime, endTime;

    clock_gettime(CLOCK_REALTIME,&startTime);
    
    task();

    clock_gettime(CLOCK_REALTIME,&endTime);

    printf("Duration: %.9fs\n",getDuration(&startTime,&endTime));

    return 0;
}

double getDuration(struct timespec *start, struct timespec *end){
    
    double duration;

    duration=((double)(end->tv_sec-start->tv_sec))
        +((double)(end->tv_nsec-start->tv_nsec))/BILLION;
    
    return duration;
}

int task(){

    void *handle=dlopen("./libmycode.so",RTLD_LAZY);

    if(!handle){
        printf("There is no libmycode.so in current directory.\n");
        return -1;
    }

    int (*myFunction)();
    myFunction=(int (*)())dlsym(handle,"isPerfectNumber");

    if(dlerror()!=NULL){
        printf("There is no such a method in the library.\n");
        return -1;
    }

    int j=0;
    int *perfectNumbers=malloc(SIZE*sizeof(int));
    
    for(int i=2; i<NUMBER; i++){
        
        if((*myFunction)(i)){
            perfectNumbers[j++]=i;
        }
    }

    dlclose(handle);

    //printNumbers(perfectNumbers);
    free(perfectNumbers);
    return 0;
}

void printNumbers(int *perfectNumbers){

    printf("Liczby doskonale: ");
    for(int j=0; j<SIZE; j++){
        printf("%d\t",perfectNumbers[j]);
    }
    printf("\n");
}