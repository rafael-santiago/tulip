/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_DSL_COMPILER_VERIFIERS_TAPPING_H
#define TULIP_DSL_COMPILER_VERIFIERS_TAPPING_H 1

#include <base/types.h>

int tapping_sep_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

#endif
