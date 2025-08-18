
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"

int numba = 2;
int *pointa = &numba;


int main () {
    printf("%02d \n", numba);
    char numbamem[20];
    sprintf(numbamem, "%s", &numba);
    printf("%d \n", numbamem);
    printf("%02d \n", *pointa);
}
