/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/printers/transcribertxtprinter.h>
#include <dsl/str/strutils.h>

void txttypesetter_transcriber_printer(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note) {
    txttypesetter_tablature_ctx *tp = NULL;
    if (tab == NULL || note == NULL || (*tab) == NULL) {
        return;
    }

    for (tp = (*tab); tp->last != NULL; tp = tp->last);

    tp->transcriber = expand_string(note->buf);
}
