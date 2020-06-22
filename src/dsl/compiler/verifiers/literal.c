/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/literal.h>
#include <dsl/compiler/compiler.h>
#include <dsl/utils.h>
#include <base/memory.h>
#include <base/ctx.h>
#include <dsl/parser/parser.h>
#include <dsl/str/strutils.h>
#include <string.h>

int literal_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    tulip_single_note_ctx *sp = NULL;
    const char *bp = NULL, *bp_end = NULL;
    char *string = NULL;
    int is_ok = 1;
    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }

    if (get_cmd_code_from_cmd_tag(buf) != kTlpLiteral) {
        tlperr_s(error_message, "A tag .literal was expected.");
        return 0;
    }

    if (get_used_techniques() != kTlpNone) {
        tlperr_s(error_message, "The tag .literal can not be combined with another tag.");
        return 0;
    }

    bp = get_next_tlp_technique_block_begin(buf);

    if (bp == NULL) {
        tlperr_s(error_message, "Tag .literal without code listing.");
        return 0;
    }

    bp_end = get_next_tlp_technique_block_end(buf);

    if (bp_end == NULL) {
        tlperr_s(error_message, "Unterminated .literal tag.");
        return 0;
    }

    push_technique(kTlpLiteral);

    string = (char *) getseg(bp_end - bp + 1);
    memset(string, 0, bp_end - bp + 1);
    memcpy(string, bp + 1, bp_end - bp - 1);

    if (!is_valid_string(string)) {
        tlperr_s(error_message, "Invalid string: %s", string);
        is_ok = 0;
    } else {
        (*song) = add_note_to_tulip_single_note_ctx(song, kTlpLiteral, string);
        (*next) = bp_end;
    }

    free(string);

    return is_ok;
}
