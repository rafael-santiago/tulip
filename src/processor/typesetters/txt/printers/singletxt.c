/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/printers/singletxt.h>
#include <processor/typesetters/txt/printers/txtprinterflush.h>
#include <processor/typesetters/txt/txt.h>
#include <stdlib.h>

void txttypesetter_single_printer(const tulip_command_t technique, txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note) {
    int row_usage = 0;
    txttypesetter_tablature_ctx *tp = NULL;

    if (tab == NULL || note == NULL) {
        return;
    }

    row_usage = txttypesetter_eval_buffer_row_usage(note->techniques, note, (*tab));
    tp = txttypesetter_get_properly_output_location(tab, note, row_usage);
    txttypesetter_flush_printer(technique, &tp, note, row_usage);
}
