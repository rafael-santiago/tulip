/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/chordextsep.h>
#include <dsl/utils.h>
#include <base/ctx.h>
#include <stdio.h>

int can_apply_external_chord_separator(const tulip_single_note_ctx *song) {
    const tulip_single_note_ctx *sp = NULL;

    if (song == NULL) {
        return 0;
    }

    sp = song;

    while (sp->next != NULL) {
        sp = sp->next;
    }

    if ((sp->techniques & kTlpBlockEnd) == 0) {
        return 0;
    }

    sp = sp->last;

    if (sp == NULL || (sp->techniques & kTlpChord) == 0) {
        return 0;
    }

    return 1;
}

void expand_chordextsep(const tulip_command_t separator, tulip_single_note_ctx **song) {
    tulip_command_t root_techniques = kTlpNone;
    tulip_single_note_ctx *sp = NULL;
    char buf[255] = "";

    if (song == NULL || (*song) == NULL) {
        return;
    }

    sp = (*song);

    while (sp->next != NULL) {
        sp = sp->next;
    }

    if ((sp->techniques & kTlpBlockEnd) == 0) {
        return;
    }

    sp = sp->last;

    if ((sp->techniques & kTlpChord) == 0) {
        return;
    }

    //  WARN(Santiago): Otherwise we will get an anomalous behavior during the typesetting.
    root_techniques = ( sp->techniques & (~(kTlpNoteSep | kTlpSingleNote)) );

    while (sp != NULL && sp->techniques & kTlpChord) {
        sp = sp->last;
    }

    //  INFO(Santiago): Let's build up the virtual .chord{n:sep...}.

    if (sp == NULL) {
        sp = (*song);
        if ((sp->techniques & kTlpChord) == 0) {
            return;
        }
    } else {
        sp = sp->next;
    }

    //  INFO(Santiago): For sure that this is a chord beginning.
    (*song) = add_note_to_tulip_single_note_ctx((*song), sp->techniques, NULL);
    sp = sp->next;
    while (sp->techniques & kTlpChord) {
        if (sp->techniques & kTlpSingleNote) {
            sprintf(buf, "%c:", sp->buf[0]);
            (*song) = add_note_to_tulip_single_note_ctx((*song), root_techniques | kTlpSingleNote, buf);
            (*song) = add_note_to_tulip_single_note_ctx((*song), root_techniques | separator, NULL);
        }
        sp = sp->next;
    }
    (*song) = add_note_to_tulip_single_note_ctx((*song), kTlpBlockEnd, NULL);
}
