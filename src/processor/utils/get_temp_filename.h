/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSOR_UTILS_GET_TEMP_FILENAME_H
#define TULIP_PROCESSOR_UTILS_GET_TEMP_FILENAME_H 1

#include <stdlib.h>

char *get_temp_filename(char *buf, const size_t buf_size, const char *prefix, const size_t prefix_size);

#endif
