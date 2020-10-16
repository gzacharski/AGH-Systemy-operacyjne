//Zadanie 1
//Program 1 z 4
//Program tworzy zmienna sum, a nastepnie 
//wykonuje operacje na niej dodajac lub odejmujac kolejne liczby.

#include <stdio.h>
#include <time.h>
#define BILLION 1000000000L

double getDuration(struct timespec *, struct timespec *);
void task();

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
    +((double)(end->tv_nsec-start->tv_nsec))/1000000000L;
    
    return duration;
}

void task(){
    int sum=0;
    
    for(int i=0; i<2000000000; i++){

        if(i%2==0){
            sum+=i;
        }else{
            sum-=i;
        }
    }
}