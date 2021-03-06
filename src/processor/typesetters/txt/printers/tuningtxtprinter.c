/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/printers/tuningtxtprinter.h>
#include <processor/typesetters/txt/txt.h>

void txttypesetter_tuning_printer(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note) {
    txttypesetter_tablature_ctx *tp = NULL;
    size_t t = 0, s = 0, n = 0;
    if (tab == NULL || note == NULL || note->next == NULL) {
        return;
    }
    //  INFO(Santiago): We must set the tuning for the next effective output. The .tuning tag by itself
    //                  does not consume any real output buffer amount, for this reason we need to look for this
    //                  consumption using the next technique. If the next one is another .tuning it does not matter
    //                  in some point of the processing we will set a non-dummy tuning.
    tp = txttypesetter_get_properly_output_location((tab), note, txttypesetter_eval_buffer_row_usage(note->next->techniques,
                                                                                                     note->next,
                                                                                                     (*tab)));
    t = 0;
    s = (*tab)->string_nr - 1;
    n = 0;
    while (note->buf[n] != 0 && s >= 0) {
        if (note->buf[n] == '-') {
            t = 0;
            s--;
        } else {
            tp->tuning[s][t++] = note->buf[n];
            tp->tuning[s][t] = 0;
        }
        n++;
    }
}
