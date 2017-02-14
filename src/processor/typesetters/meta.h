/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSORS_TYPESETTERS_META_H
#define TULIP_PROCESSORS_TYPESETTERS_META_H 1

#include <base/types.h>
#include <stdio.h>

typedef void (*song_title_spill_func)(FILE *fp, const char *title);
typedef void (*transcribers_name_spill_func)(FILE *fp, const char *title);
typedef void (*tab_notation_spill_func)(FILE *fp, const tulip_single_note_ctx *song);
typedef void (*tunning_spill_func)(FILE *fp);
typedef void (*song_spill_func)(FILE *fp, const tulip_single_note_ctx *song);

int metatypesetter_inkspill(const char *tabpath, const tulip_single_note_ctx *song, const char *linebreaking,
                            song_title_spill_func metatypesetter_spill_song_title,
                            transcribers_name_spill_func metatypesetter_spill_transcribers_name,
                            tab_notation_spill_func metatypesetter_spill_tab_notation,
                            tunning_spill_func metatypesetter_spill_tunning,
                            song_spill_func metatypesetter_spill_song);

#endif
