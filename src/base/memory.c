#include "memory.h"
#include <stdio.h>
#include <unistd.h>

void *getseg(const size_t ssize) {
    void *p = NULL;
    p = malloc(ssize);
    if (p == NULL) {
        printf("tulip PANIC: no memory!\n");
        exit(1);
    }
    return p;
}
