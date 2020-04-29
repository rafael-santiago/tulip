/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/utils/has_convert.h>
#include <stdio.h>

int has_convert(void) {
    FILE *proc = popen("convert --version", "r");
    int has = (proc != NULL);
    if (has) {
        pclose(proc);
    }
    return has;
}
