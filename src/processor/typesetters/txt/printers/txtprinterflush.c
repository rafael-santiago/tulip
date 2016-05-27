/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/printers/txtprinterflush.h>
#include <processor/oututils.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

static void txttypesetter_flush_note(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note, const int row_usage);

static void txttypesetter_flush_note(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note, const int row_usage) {
    char s_fret_nr[255] = "";
    if (tab == NULL || note == NULL) {
        return;
    }

    (*tab)->curr_str = (note->buf[0] - '0');

    if (isdigit(note->buf[0]) && note->buf[1] == ':') {
        return;
    }

    if (note->buf[1] == 'X' || note->buf[1] == '?') {
        (*tab)->strings[(*tab)->curr_str][(*tab)->curr_row] = note->buf[1];
    } else {
        sprintf(s_fret_nr, "%d", single_note_to_tab_fret_nr(note->buf));
        strncat((*tab)->strings[(*tab)->curr_str], s_fret_nr, (*tab)->fretboard_sz);
    }

    if ((note->techniques & kTlpChord) == 0 || (note->next != NULL && (note->next->techniques & kTlpChord) == 0)) {
        (*tab)->curr_row += row_usage;
    }
}

void txttypesetter_flush_printer(const tulip_command_t command, txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note, const int row_usage) {
    char *technique_label = NULL;

    if (tab == NULL || note == NULL) {
        return;
    }

    switch (command) {
        case kTlpVibrato:
        case kTlpSlideDown:
        case kTlpSlideUp:
        case kTlpHammerOn:
        case kTlpPullOff:
        case kTlpNoteSep:
        case kTlpSepBar:
        case kTlpBend:
        case kTlpReleaseBend:
        case kTlpTapping:
        case kTlpNaturalHarmonic:
        case kTlpArtificialHarmonic:
            technique_label = get_technique_label(command);
            if (technique_label != NULL) {
                strncat((*tab)->strings[(*tab)->curr_str], technique_label, (*tab)->fretboard_sz);
            }
            if ((note->techniques & kTlpChord) == 0) {
                (*tab)->curr_row += row_usage;
            }
            break;

        case kTlpSingleNote:
            txttypesetter_flush_note(tab, note, row_usage);
            break;

        case kTlpSavePoint:
            new_txttypesetter_tablature_ctx(tab);
            break;
    }

}
