//Zadanie 1 
//Program 4 z 4
//Program wypisuje liczby doskonale w zakresie NUMBER.
//Liczba doskonala to taka liczba, ktorej suma dzielnikow, oprocz niej samej,
//jest rowna rozpatrywanej liczbie.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L
#define NUMBER 100000
#define SIZE 4

void task(int);
double getDuration(struct timespec *, struct timespec *);
void printNumbers(int *);
int isPerfectNumber(int);

int main(int argc, char *argv[]){

    int numberOfIteration=0;

    if(argc==2){
        printf("Supplied argument is %s.\n", argv[1]);
        numberOfIteration=atoi(argv[1]);
    }else{
        numberOfIteration=NUMBER;
    }
    printf("Number of iteration: %d.\n", numberOfIteration);

    struct timespec startTime, endTime;

    clock_gettime(CLOCK_REALTIME,&startTime);
    
    task(numberOfIteration);

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

void task(int numberOfIteration){

    int j=0;
    int *perfectNumbers=malloc(SIZE*sizeof(int));
    
    for(int i=2; i<numberOfIteration; i++){
        
        if(isPerfectNumber(i)){
            perfectNumbers[j++]=i;
        }
    }

    //printNumbers(perfectNumbers);
    free(perfectNumbers);
}

int isPerfectNumber(int number){
    int divider=1;
    int sumOfDividers=0;

    while(divider<=number/2 && sumOfDividers<=number){
        if(number%divider==0){
            sumOfDividers+=divider;
        }
        divider++;
    }

    return (sumOfDividers==number)?1:0;
}

void printNumbers(int *perfectNumbers){

    printf("Liczby doskonale: ");
    for(int j=0; j<SIZE; j++){
        printf("%d\t",perfectNumbers[j]);
    }
    printf("\n");
}