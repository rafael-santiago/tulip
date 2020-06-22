/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/datatag.h>
#include <dsl/compiler/compiler.h>
#include <dsl/str/strutils.h>
#include <dsl/utils.h>
#include <dsl/parser/parser.h>
#include <base/ctx.h>
#include <string.h>

int datatag_verifier(const tulip_command_t command, const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    const char *bp = NULL, *bp_end = NULL;
    char string[255] = "";
    const char *tag = NULL;

    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }

    if (get_cmd_code_from_cmd_tag(buf) != command) {
        tag = get_cmd_tag_from_cmd_code(command);
        tlperr_s(error_message, "A tag %s was expected.", (tag == NULL || strlen(tag) == 1) ? tag : tag + 1);
        return 0;
    }

    bp = get_next_tlp_technique_block_begin(buf);

    if (bp == NULL) {
        tag = get_cmd_tag_from_cmd_code(command);
        tlperr_s(error_message, "A tag %s without code listing.", (tag == NULL || strlen(tag) == 1) ? tag : tag + 1);
        return 0;
    }

    bp_end = get_next_tlp_technique_block_end(buf);

    if (bp_end == NULL) {
        tag = get_cmd_tag_from_cmd_code(command);
        tlperr_s(error_message, "Unterminated %s tag.", (tag == NULL || strlen(tag) == 1) ? tag : tag + 1);
        return 0;
    }

    bp++;
    memset(string, 0, sizeof(string));
    memcpy(string, bp, (bp_end - bp) % sizeof(string));

    if (!is_valid_string(string)) {
        tlperr_s(error_message, "Invalid string : %s\n", string);
        return 0;
    }

    (*song) = add_note_to_tulip_single_note_ctx(song, command, string);
    (*next) = bp_end + 1;

    return 1;

}
