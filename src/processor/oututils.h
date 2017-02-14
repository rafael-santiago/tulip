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
#include <processor/typesetters/txt/txttypes.h>
#include <stdlib.h>

char *get_technique_label(const tulip_command_t command);

char *get_technique_notation_label(const tulip_command_t command);

int single_note_to_tab_fret_nr(const char *single_note);

int is_tab_empty(const txttypesetter_tablature_ctx *tab);

int tunning_has_half_step_notes(const txttypesetter_tablature_ctx *tab, const char tunning_buffer[6][4], const tulip_prefs_map_t prefs);

ssize_t get_fretboard_usage_limit(const txttypesetter_tablature_ctx *tab);

void get_all_used_techniques(const tulip_single_note_ctx *song, tulip_command_t used_techniques[31], size_t *used_techniques_nr, int *has_muffled, int *has_anyfret);

char *tlptemp(void);

char *typesetter_raw_output(const tulip_single_note_ctx *song, size_t *osize, int (*typesetter)(const tulip_single_note_ctx *song, const char *tempfile));

#endif
