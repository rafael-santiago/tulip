/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/printers/chordtxtprinter.h>
#include <processor/typesetters/txt/txt.h>
#include <processor/typesetters/txt/txtctx.h>
#include <stdlib.h>

void txttypesetter_chord_printer(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note) {
    txttypesetter_tablature_ctx *tp = NULL;
    if (tab == NULL || note == NULL) {
        return;
    }
    //  WARN(Santiago): This is pretty dumb. It will be stripped off soon.
//    tp = txttypesetter_get_properly_output_location(tab, txttypesetter_eval_buffer_row_usage(kTlpChord, note, (*tab)));
//    tp->techniques = push_technique_to_txttypesetter_sustained_technique_ctx(tp->techniques, kTlpChord);
}
