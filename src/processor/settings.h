/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSOR_SETTINGS_H
#define TULIP_PROCESSOR_SETTINGS_H 1

#include <stdlib.h>

void set_processor_setting(const char *setting, const void *data, const size_t dsize);

void *get_processor_setting(const char *setting, size_t *dsize);

#endif
