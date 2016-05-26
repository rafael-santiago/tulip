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
            break;

        case kTlpSavePoint:
            break;
    }
}