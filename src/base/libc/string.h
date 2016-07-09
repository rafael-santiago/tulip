/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_BASE_LIBC_STRING_H
#define TULIP_BASE_LIBC_STRING_H 1

#ifdef _WIN32

#include <stdlib.h>

size_t strnlen(const char *str, const size_t maxlen);

#endif

#include <string.h>

#endif
