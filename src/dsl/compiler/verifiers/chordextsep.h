/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_DSL_COMPILER_VERIFIERS_CHORDEXTSEP_H
#define TULIP_DSL_COMPILER_VERIFIERS_CHORDEXTSEP_H 1

#include <base/types.h>

int can_apply_external_chord_separator(const tulip_single_note_ctx *song);

void expand_chordextsep(const tulip_command_t separator, tulip_single_note_ctx **song);

#endif
