/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSOR_TYPESETTERS_TYPEPREFS_H
#define TULIP_PROCESSOR_TYPESETTERS_TYPEPREFS_H 1

#include <base/types.h>
#include <stdlib.h>

struct typesetter_curr_settings {
    tulip_prefs_map_t prefs;
    size_t indentation_deepness;
};

struct typesetter_curr_settings  typesetter_settings(void);

#endif
