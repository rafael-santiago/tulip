/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/printers/literaltxtprinter.h>
#include <processor/typesetters/txt/txt.h>
#include <processor/typesetters/txt/txtctx.h>
#include <dsl/str/strutils.h>

void txttypesetter_literal_printer(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note) {
    char *string = NULL;
    txttypesetter_tablature_ctx *tp = NULL;
    if (tab == NULL || note == NULL) {
        return;
    }
    string = expand_string(note->buf);
    if (string == NULL) {
        return;
    }
    tp = txttypesetter_get_properly_output_location(tab, 0);
    tp->comments = add_comment_to_txttypesetter_comment_ctx(tp->comments, string, (*tab)->fretboard_sz);
    free(string);
}
