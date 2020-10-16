//Zadanie 1
//Program 3 z 4
//Program tworzy tablice o rozmiarze SIZE i wpisuje do niej
//kolejne liczby ciagu Fibonacciego, uzywajac funkcji rekurencyjnej (zabieg celowy).

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L
#define SIZE 45

void task();
double getDuration(struct timespec *, struct timespec *);
int fib(int n);

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

void task(){
    
    int *tab=malloc(SIZE*sizeof(int));

    for(int i=0; i<SIZE; i++){
        tab[i]=fib(i);
    }

    free(tab);
}

int fib(int n){
    if(n==0) return 0;
    if(n==1) return 1;

    return fib(n-1)+fib(n-2);
}