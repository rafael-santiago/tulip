/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <base/libc/string.h>

#ifdef _WIN32

//  WARN(Santiago): Certain MINGW's versions does not have this so let's garantee this function by ourselves.

size_t strnlen(const char *str, const size_t maxlen) {
    const char *sp = NULL;
    const char *sp_end = NULL;
    
    if (str == NULL) {
        return 0;
    }
    
    sp = str;
    sp_end = str + maxlen;
    while (sp != sp_end && *sp != 0) {
        sp++;
    }
    return (sp - str);
}

#endif
