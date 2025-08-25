
#include <iostream>
//#include <

#include "test.h"

int numba = 2;
int numba2 = 20;
int *pointa = &numba;
int *pointa2 = &numba2;
int *pointa3 = pointa2;


void printNumbers (int *num) {
    for (size_t i = 0; i < 5; i++)
    {
        std::cout<<"num = "<< *num <<"\n";
        *num += 1;
    }
    std::cout<<"\n";
}

//void (*printNumbersPointer)(int) = &printNumbers;


int main () {
    std::cout<<"numba = "<< numba <<"\n";
    std::cout<<"&numba = "<< &numba <<"\n";
    std::cout<<"*&numba = "<< *&numba <<"\n";
    std::cout<<"pointa = "<< pointa <<"\n";
    std::cout<<"*pointa = "<< *pointa <<"\n";
    std::cout<<"&pointa = "<< &pointa <<"\n\n";
    std::cout<<"pointa2 = "<< pointa2 <<"\n";
    std::cout<<"*pointa2 = "<< *pointa2 <<"\n";
    std::cout<<"&pointa2 = "<< &pointa2 <<"\n\n";
    std::cout<<"pointa3 = "<< pointa3 <<"\n";
    std::cout<<"*pointa3 = "<< *pointa3 <<"\n";
    std::cout<<"&pointa3 = "<< &pointa3 <<"\n\n";

    printNumbers(&numba);
    printNumbers(pointa);
//    printNumbersPointer(*pointa);

    printNumbers(&numba2);
    printNumbers(pointa2);
    printNumbers(pointa3);
}
