/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/printers/songtxtprinter.h>
#include <processor/typesetters/txt/txt.h>
#include <dsl/str/strutils.h>

void txttypesetter_song_printer(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note) {
    txttypesetter_tablature_ctx *tp = NULL;

    if (tab == NULL || note == NULL) {
        return;
    }

    if ((*tab) != NULL) {
        for (tp = (*tab); tp->last != NULL; tp = tp->last);
    } else {
        tp = txttypesetter_get_properly_output_location(tab, 0);
    }

    tp->song = expand_string(note->buf);
}
