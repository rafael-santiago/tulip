/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/notesep.h>
#include <dsl/compiler/compiler.h>
#include <dsl/utils.h>
#include <base/ctx.h>

int notesep_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }

    if (get_cmd_code_from_cmd_tag(buf) != kTlpNoteSep) {
        tlperr_s(error_message, "A single note separator was expected.");
        return 0;
    }

    (*song) = add_note_to_tulip_single_note_ctx(song, get_used_techniques() | kTlpNoteSep, NULL);
    (*next) = buf + 1;

    return 1;
}
