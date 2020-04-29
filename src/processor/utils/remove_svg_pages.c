/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/utils/remove_svg_pages.h>
#include <stdio.h>

int remove_svg_pages(const char *basename) {
    char temp[4096];
    size_t pp = 1;
    int rm_nr = -1;

    do {
        rm_nr++;
        snprintf(temp, sizeof(temp) - 1, "%s-%03d.svg", basename, pp++);
    } while (remove(temp) == 0);

    return (rm_nr > 0);
}
