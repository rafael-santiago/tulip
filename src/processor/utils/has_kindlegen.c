/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/utils/has_kindlegen.h>
#include <stdlib.h>

int has_kindlegen(void) {
#if defined(__unix__)
    return (system("kindlegen > /dev/null 2>&1") == 0);
#elif defined(_WIN32)
    return (system("kindlegen > nul 2>&1") == 0);
#else
# error Some code wanted.
#endif
}
