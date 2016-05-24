/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSOR_OUTUTILS_H
#define TULIP_PROCESSOR_OUTUTILS_H 1

#include <base/types.h>

char *get_technique_label(const tulip_command_t command);

int single_note_to_tab_fret_nr(const char *single_note);

#endif
