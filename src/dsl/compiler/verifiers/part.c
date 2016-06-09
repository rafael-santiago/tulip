/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/part.h>
#include <dsl/compiler/verifiers/oncemore.h>
#include <dsl/compiler/compiler.h>
#include <dsl/utils.h>
#include <dsl/parser/parser.h>
#include <base/ctx.h>
#include <ctype.h>
#include <string.h>

int part_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    const char *bp = NULL, *bp_end = NULL;
    char label[255] = "";
    tulip_single_note_ctx *begin = NULL, *end = NULL;

    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }

    if (get_cmd_code_from_cmd_tag(buf) != kTlpPart) {
        tlperr_s(error_message, "A part tag was expected.");
        return 0;
    }

    bp = get_next_tlp_technique_block_begin(buf);

    if (bp == NULL) {
        tlperr_s(error_message, "A tag part without code listing.");
        return 0;
    }

    bp_end = get_next_tlp_technique_block_end(buf);

    if (bp_end == NULL) {
        tlperr_s(error_message, "Unterminated part tag.");
        return 0;
    }

    memset(label, 0, sizeof(label));
    bp++;
    memcpy(label, bp, (bp_end - bp) % sizeof(label));

    while (bp != bp_end) {
        if (!(isascii(*bp) && !is_string_delim(*bp) && !is_technique_block_begin(*bp) && !is_technique_block_end(*bp))) {
            tlperr_s(error_message, "The part label has a invalid character : %s", label);
            return 0;
        }
        bp++;
    }

    if (get_tulip_part_ctx(label, get_parts_listing()) != NULL) {
        tlperr_s(error_message, "The part \"%s\" is being redeclared.", label);
    }

    for (end = (*song); end->next != NULL; end = end->next);

    if (end->last == NULL) {
        tlperr_s(error_message, "Insufficient notes to make a part called \"%s\".", label);
        return 0;
    }

    begin = find_oncemore_begin(end);

    set_parts_listing(add_part_to_tulip_part_ctx(get_parts_listing(), label, begin, end));

    (*next) = bp_end + 1;

    return 1;
}
