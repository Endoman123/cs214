#include <stdlib.h>
#include <stdio.h>

#include "mymalloc.h"

int main(int argc, char **argv) {
    //A: malloc() 1 byte and immediately free it - do this 150 times
    int i; for (i = 0; i < 150; i++) free(malloc(1));
}
