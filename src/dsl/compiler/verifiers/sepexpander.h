/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_DSL_COMPILER_VERIFIERS_SEPEXPANDER_H
#define TULIP_DSL_COMPILER_VERIFIERS_SEPEXPANDER_H 1

#include <dsl/compiler/verifiers/chordextsep.h>
#include <base/ctx.h>

#define add_sep_to_tulip_single_note_ctx(sep, song) {\
    if (!can_apply_external_chord_separator(*(song))) {\
        (*(song)) = add_note_to_tulip_single_note_ctx(song, get_used_techniques() | (sep), NULL);\
    } else {\
        expand_chordextsep((sep), (song));\
    }\
}

#endif
