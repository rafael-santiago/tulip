/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/tuning.h>
#include <dsl/compiler/compiler.h>
#include <dsl/utils.h>
#include <dsl/parser/parser.h>
#include <base/ctx.h>
#include <ctype.h>
#include <string.h>

int tuning_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    const char *bp = NULL, *bp_end = NULL;
    char cipher[255] = "";
    char tuning[255] = "";
    char *cp = NULL, *cp_end = NULL;

    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }

    if (get_cmd_code_from_cmd_tag(buf) != kTlpTuning) {
        tlperr_s(error_message, "The tag tuning was expected.");
        return 0;
    }

    bp = get_next_tlp_technique_block_begin(buf);

    if (bp == NULL) {
        tlperr_s(error_message, "A tag tuning without code listing.");
        return 0;
    }

    bp_end = get_next_tlp_technique_block_end(buf);

    if (bp_end == NULL) {
        tlperr_s(error_message, "Unterminated tag tuning.");
        return 0;
    }

    cp = &cipher[0];
    cp_end = cp + sizeof(cipher);
    bp++;
    while (bp != bp_end) {
        if (cp == cp_end) {
            *cp = 0;
            tlperr_s(error_message, "Too long tuning sequence: %s.", cipher);
            return 0;
        }
        if (is_sep(*bp) || (bp + 1) == bp_end) {
            if ((bp + 1) == bp_end) {
                *cp = *bp;
                cp++;
            }
            *cp = 0;
            if (!is_valid_note_cipher(cipher)) {
                tlperr_s(error_message, "Invalid note cipher: %s.", cipher);
                return 0;
            }
            strncat(tuning, cipher, sizeof(tuning) - 1);
            strncat(tuning, "-", sizeof(tuning) - 1);
            cp = &cipher[0];
        } else {
            *cp = *bp;
            cp++;
        }
        bp++;
    }

    (*song) = add_note_to_tulip_single_note_ctx((*song), kTlpTuning, tuning);
    (*next) = bp_end + 1;

    return 1;
}
