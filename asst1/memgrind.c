#include <stdlib.h>
#include <stdio.h>

#include "mymalloc.h"

int main(int argc, char **argv) {
    int test = 12;

    test = (int)setBit((unsigned long)test, 1, 0); 
    printf("%d\n", test);
    
    test = (int)getBits((unsigned long)test, 3, 1);
    printf("%d\n", test);

    return 0;
}
