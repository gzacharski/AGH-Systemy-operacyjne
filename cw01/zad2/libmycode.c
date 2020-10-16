#include "mycode.h"

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