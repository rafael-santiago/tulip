/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/times.h>
#include <dsl/utils.h>
#include <dsl/compiler/compiler.h>
#include <dsl/parser/parser.h>
#include <base/ctx.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int times_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    const char *bp = NULL, *bp_end = NULL;
    char times[255], *tp = NULL, *tp_end = NULL;

    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }

    if (get_cmd_code_from_cmd_tag(buf) != kTlpTimes) {
        tlperr_s(error_message, "A tag times was expected.");
        return 0;
    }

    bp = get_next_tlp_technique_block_begin(buf);

    if (bp == NULL) {
        tlperr_s(error_message, "A tag times without code listing.");
        return 0;
    }

    bp_end = get_next_tlp_technique_block_end(buf);

    if (bp_end == NULL) {
        tlperr_s(error_message, "Unterminated tag times.");
        return 0;
    }

    if (bp == bp_end) {
        tlperr_s(error_message, "A valid number of times was expected.");
        return 0;
    }

    tp = &times[0];
    tp_end = tp + sizeof(times) - 2;
    memset(tp, 0, sizeof(times));
    bp++;

    while (bp != bp_end) {
        if (!isdigit(*bp)) {
            tlperr_s(error_message, "A valid number of times was expected.");
            return 0;
        }
        if (tp == tp_end) {
            tlperr_s(error_message, "The specified amount of time seems to be too long. Aborting the compilation.");
            return 0;
        }
        *tp = *bp;
        tp++;
        bp++;
    }

    if (atoi(times) <= 0) {
        tlperr_s(error_message, "Dummy amount of time.");
        return 0;
    }

    strncat(times, "X", sizeof(times) - 1);

    (*song) = add_note_to_tulip_single_note_ctx(song, get_used_techniques() | kTlpTimes, times);
    (*next) = bp_end + 1;

    return 1;
}
