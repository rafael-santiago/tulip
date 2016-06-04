/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/printers/sustainedtxt.h>
#include <processor/typesetters/txt/txt.h>

void txttypesetter_sustained_printer(const tulip_command_t technique, txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note) {
    txttypesetter_tablature_ctx *tp = NULL;
    int row_usage = 0;
    if (tab == NULL || note == NULL) {
        return;
    }
    if (note->techniques & kTlpSepBar) {
        return;
    }
    if ((note->techniques & kTlpChord) && note->next != NULL && (note->next->techniques & kTlpChord)) {
        return;
    }
    row_usage = txttypesetter_eval_buffer_row_usage(note->techniques, note, *tab);
    tp = txttypesetter_get_properly_output_location(tab, row_usage);
    txttypesetter_print_sustained_technique_mark(technique, tab, row_usage);
}
