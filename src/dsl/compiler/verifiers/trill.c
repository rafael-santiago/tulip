/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/trill.h>
#include <dsl/compiler/verifiers/undo_recursion.h>
#include <dsl/utils.h>

int trill_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }
    if (get_cmd_code_from_cmd_tag(buf) != kTlpTrill) {
        tlperr_s(error_message, "The .trill tag was expected.");
        return 0;
    }
    return undo_recursion(kTlpTrill, buf, error_message, song, next);
}
