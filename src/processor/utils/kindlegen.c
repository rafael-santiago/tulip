/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/utils/kindlegen.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#if defined(_WIN32)
# include <windows.h>
#endif

int kindlegen(const char *opf_filepath, const char *outpath) {
    char cmdline[4096], *cp;
    int has_error = 1;
#if defined(__unix__)
    snprintf(cmdline, sizeof(cmdline) - 1, "kindlegen %s > /dev/null 2>&1", opf_filepath);
#elif defined(_WIN32)
    snprintf(cmdline, sizeof(cmdline) - 1, "kindlegen %s > nul 2>&1", opf_filepath);
#else
# error Some code wanted.
#endif

    system(cmdline); // INFO(Rafael): Kindlegen is rather pedantic let's ignore its complaints.

    snprintf(cmdline, sizeof(cmdline) - 1, "%s", opf_filepath);
    if ((cp = strstr(cmdline, ".opf")) != NULL) {
        *cp = 0;
    }

    memcpy(&cmdline[0] + strlen(cmdline), ".mobi\0", 6);

#if defined(__unix__)
    if ((has_error = rename(cmdline, outpath)) != 0) {
        fprintf(stderr, "ERROR: Unable to generate .mobi file.\n");
    }
#elif defined(_WIN32)
    if ((has_error = MoveFileEx(cmdline, outpath, MOVEFILE_REPLACE_EXISTING) == 0)) {
        fprintf(stderr, "ERROR: Unable to generate .mobi file.\n");
    }
#else
# error Some code wanted.
#endif

    return has_error;
}
