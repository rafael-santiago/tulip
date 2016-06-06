/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/printers/txtprinterflush.h>
#include <processor/typesetters/txt/txt.h>
#include <processor/oututils.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

static void txttypesetter_flush_note(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note, const int row_usage);

static void ttxttypesetter_flush_sustained_marks(txttypesetter_tablature_ctx **tab, const int row_usage);

static void txttypesetter_flush_note(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note, const int row_usage) {
    char s_fret_nr[255] = "";
    char *string = NULL;

    if (tab == NULL || note == NULL) {
        return;
    }

    (*tab)->curr_str = (note->buf[0] - '0') - 1;

    if (isdigit(note->buf[0]) && note->buf[1] == ':') {
        return;
    }

    if ((*tab)->active_techniques != NULL) {
        ttxttypesetter_flush_sustained_marks(tab, row_usage);
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

static void ttxttypesetter_flush_sustained_marks(txttypesetter_tablature_ctx **tab, const int row_usage) {
    int temp = 0;
    txttypesetter_active_technique_ctx *ap = NULL;
    if (tab == NULL) {
        return;
    }
    for (ap = (*tab)->active_techniques; ap != NULL; ap = ap->next) {
        temp = (*tab)->curr_row;
        txttypesetter_print_sustained_technique_mark(ap->technique, tab, row_usage);
        (*tab)->curr_row = temp;
    }
}

void txttypesetter_flush_printer(const tulip_command_t command, txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note, const int row_usage) {
    char *technique_label = NULL, *tp = NULL;
    char *string = NULL;
    size_t s = 0;

    if (tab == NULL || note == NULL) {
        return;
    }

    if (command == kTlpNoteSep && (*tab)->curr_row == 1) {
        //  INFO(Santiago): This avoid the row consumption with a single '-' at the beginning of
        //                  the tablature what would be a poor output.
        return;
    }

    switch (command) {
        case kTlpVibrato:
        case kTlpSlideDown:
        case kTlpSlideUp:
        case kTlpHammerOn:
        case kTlpPullOff:
        case kTlpNoteSep:
        case kTlpBend:
        case kTlpReleaseBend:
        case kTlpTapping:
        case kTlpNaturalHarmonic:
        case kTlpArtificialHarmonic:
            //  INFO(Santiago): This avoid erasing when we have a chord in printing process. Note that separators
            //                  inside chord tags must be "pinched" on the fretboard using string jumps.
            if (note->techniques & kTlpChord) {
                return;
            }

            technique_label = get_technique_label(command);

            if (technique_label == NULL) {
                return;
            }

            if (!((*tab)->curr_str >= 0 && (*tab)->curr_str < 6)) {
                return;
            }

            memcpy(&(*tab)->strings[(*tab)->curr_str][(*tab)->curr_row], technique_label, strlen(technique_label));

            if ((*tab)->active_techniques != NULL) {
                ttxttypesetter_flush_sustained_marks(tab, row_usage);
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

        case kTlpSepBar:
            if (note->techniques & kTlpChord) {
                //  INFO(Santiago): Step out! It does not make sense.
                return;
            }

            technique_label = get_technique_label(command);

            if (technique_label == NULL) {
                return;
            }

            for (s = 0; s < 6; s++) {
                (*tab)->strings[s][(*tab)->curr_row] = *technique_label;
            }

            (*tab)->curr_row += row_usage;
            break;

        case kTlpTimes:
            memcpy(&(*tab)->times[(*tab)->curr_row], note->buf, strlen(note->buf));
            break;

    }

}
