//Zadanie 1
//Program 2 z 4
//Program tworzy 2 tablice char o rozmierze SIZE
//i wpisuje do kazdej komorki losowy znak od a-z.
//Nastepnie tworzy 3 tablice i scala dwie tablice w jeden napis.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BILLION 1000000000L
#define SIZE 100000000

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
        +((double)(end->tv_nsec-start->tv_nsec))/BILLION;
    
    return duration;
}

void task(){
    
    int *tab1=malloc(SIZE*sizeof(int));
    int *tab2=malloc(SIZE*sizeof(int));
    int *tab3=malloc(SIZE*sizeof(int));

    for(int i=0; i<SIZE; i++){
        tab1[i]=rand()*100;
        tab2[i]=rand()*100;
    }

    for(int i=0; i<SIZE; i++){
        tab3[i]=tab1[i]+tab2[i];
    }

    free(tab1);
    free(tab2);
    free(tab3);
}