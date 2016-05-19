/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
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
