/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/repeat.h>
#include <dsl/compiler/verifiers/oncemore.h>
#include <dsl/compiler/compiler.h>
#include <dsl/parser/parser.h>
#include <dsl/utils.h>
#include <base/ctx.h>
#include <string.h>

int repeat_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    const char *bp = NULL, *bp_end = NULL;
    char label[255] = "";
    tulip_part_ctx *part = NULL;

    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }

    if (get_cmd_code_from_cmd_tag(buf) != kTlpRepeat) {
        tlperr_s(error_message, "A repeat tag was expected.");
        return 0;
    }

    bp = get_next_tlp_technique_block_begin(buf);
    bp_end = get_next_tlp_technique_block_end(buf);
    bp++;
    memset(label, 0, sizeof(label));
    memcpy(label, bp, (bp_end - bp) % sizeof(label));

    if ((part = get_tulip_part_ctx(label, get_parts_listing())) == NULL) {
        tlperr_s(error_message, "An undefined part \"%s\" was mentioned.", label);
        return 0;
    }

    tulip_single_note_ctx_cpy(song, part->begin, part->end);
    (*next) = bp_end + 1;

    return 1;
}
