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
    char *string = NULL;
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
        string = &(*tab)->strings[(*tab)->curr_str][(*tab)->curr_row];
        if ((note->techniques & kTlpChord) && row_usage > 1 && strlen(s_fret_nr) == 1) {
            //  INFO(Santiago): Seeking the best note alignment.
            string += (row_usage - 1);
        }
        memcpy(string, s_fret_nr, strnlen(s_fret_nr, sizeof(s_fret_nr)));
    }

    if ((note->techniques & kTlpChord) == 0 || (note->next != NULL && (note->next->techniques & kTlpChord) == 0)) {
        (*tab)->curr_row += row_usage;
    }
}

void txttypesetter_flush_printer(const tulip_command_t command, txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note, const int row_usage) {
    char *technique_label = NULL, *tp = NULL;
    char *string = NULL;

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
            memcpy(&(*tab)->strings[(*tab)->curr_str][(*tab)->curr_row], technique_label, strlen(technique_label));
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
