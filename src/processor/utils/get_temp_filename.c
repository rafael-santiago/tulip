/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/utils/get_temp_filename.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

char *get_temp_filename(char *buf, const size_t buf_size, const char *prefix, const size_t prefix_size) {
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // WARN(Rafael): It is safe to use this function only if you will use the !
    //               temp name as a part of the final file name.              !
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    char tempname[65535], *tp, *tp_head;
    size_t rnd;
    struct stat st;
    static int get_temp_filename_initd = 0;

    if (buf == NULL || buf_size == 0 || buf_size <= prefix_size) {
        return NULL;
    }

    if ((prefix == NULL && prefix_size > 0) || (prefix != NULL && prefix_size == 0)) {
        return NULL;
    }

    if (prefix_size >= buf_size || prefix_size >= sizeof(tempname)) {
        return NULL;
    }

    if (!get_temp_filename_initd) {
        srand(time(0));
        get_temp_filename_initd = 1;
    }

    memset(tempname, 0, sizeof(tempname));

    if (prefix != NULL) {
        snprintf(tempname, sizeof(tempname) - 1, "%s", prefix);
        tp_head = &tempname[0] + prefix_size;
    } else {
        tp_head = &tempname[0];
    }

    do {
        tp = tp_head;
        for (rnd = 0; rnd < 8; rnd++) {
            while (!isdigit(*tp) && !isalpha(*tp)) {
                *tp = rand() % 128;
            }
            tp++;
        }
    } while (stat(tempname, &st) == 0);

    if (tp - &tempname[0] >= buf_size) {
        return NULL;
    }

    snprintf(buf, buf_size - 1, "%s", tempname);

    return &buf[0];
}
