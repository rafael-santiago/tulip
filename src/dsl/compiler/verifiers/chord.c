/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/chord.h>
#include <dsl/utils.h>
#include <dsl/compiler/compiler.h>
#include <base/ctx.h>
#include <dsl/parser/parser.h>
#include <ctype.h>

int chord_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    const char *bp = NULL;
    const char *bp_end = NULL;
    char note[255] = "", *np, *np_end = NULL;
    tulip_single_note_ctx *sp = NULL;
    if (song == NULL || next == NULL || buf == NULL) {
        return 0;
    }
    sp = *song;
    if (get_cmd_code_from_cmd_tag(buf) != kTlpChord) {
        tlperr_s(error_message, ".chord was expected.");
        return 0;
    }
    bp = get_next_tlp_technique_block_begin(buf);
    if (bp == NULL) {
        tlperr_s(error_message, ".chord tag without note listing.");
        return 0;
    }
    bp_end = get_next_tlp_technique_block_end(buf);
    push_technique(kTlpChord);
    np = &note[0];
    np_end = np + sizeof(note);
    while (bp < bp_end) {
        //  WARN(Santiago): This tag disallow recursion so here we do not need to worry about
        //                  block ending or new tag announcement.
        if (np == np_end) {
            tlperr_s(error_message, "The current note is too long.");
            return 0;
        }
        if (is_blank(*bp)) {
            bp++;
            continue;
        }
        if (is_sep(*bp) && np != &note[0]) { // INFO(Santiago): It is legal productions like this: .chord{60-52-42---31}
            *np = 0;
            if (!is_single_note(note)) {
                tlperr_s(error_message, "Invalid note : \"%s\".", note);
            }
            sp = add_note_to_tulip_single_note_ctx(sp, get_used_techniques(), note);
            np = &note[0];
        } else if (isdigit(*bp) || is_string_jmp(*bp)) {
            *np = *bp;
            np++;
            if (is_string_jmp(*bp)) {
                bp++;
                while (is_blank(*bp)) {
                    bp++;
                }
                if (!is_note_sep(*bp)) {
                    tlperr_s(error_message, "A valid note separator was expected.");
                    return 0;
                }
                *np = *bp;
                np++;
            }
        }
        bp++;
    }
    (*next) = bp;
    return 1;
}
