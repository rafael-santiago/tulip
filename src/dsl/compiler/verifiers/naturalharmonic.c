/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/naturalharmonic.h>
#include <dsl/compiler/verifiers/sepexpander.h>
#include <dsl/compiler/compiler.h>
#include <dsl/utils.h>

int naturalharmonic_sep_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }

    if (get_cmd_code_from_cmd_tag(buf) != kTlpNaturalHarmonic) {
        tlperr_s(error_message, "A natural harmonic separator was expected.");
        return 0;
    }

    add_sep_to_tulip_single_note_ctx(kTlpNaturalHarmonic, song);

    (*next) = buf + 1;

    return 1;
}
