/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/txt.h>
#include <processor/typesetters/txt/txttypes.h>
#include <processor/typesetters/txt/txtctx.h>
#include <processor/typesetters/txt/printers/mutetxtprinter.h>
#include <processor/settings.h>
#include <processor/oututils.h>
#include <dsl/utils.h>
#include <stdlib.h>
#include <string.h>

typedef void (*txttypesetter_print_func)(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note);

#define register_new_typesetter_printer(tc, p) (p) //  INFO(Santiago): "tc" is just for documenting issues.

static txttypesetter_print_func g_txttypesetter_printers[] = {
    register_new_typesetter_printer(kTlpMute, txttypesetter_mute_printer),
    register_new_typesetter_printer(kTlpLetRing, NULL),
    register_new_typesetter_printer(kTlpChord, NULL),
    register_new_typesetter_printer(kTlpBeat, NULL),
    register_new_typesetter_printer(kTlpTremoloPicking, NULL),
    register_new_typesetter_printer(kTlpVibrato, NULL),
    register_new_typesetter_printer(kTlpSlideDown, NULL),
    register_new_typesetter_printer(kTlpSlideUp, NULL),
    register_new_typesetter_printer(kTlpHammerOn, NULL),
    register_new_typesetter_printer(kTlpPullOff, NULL),
    register_new_typesetter_printer(kTlpVibratoWBar, NULL),
    register_new_typesetter_printer(kTlpTunning, NULL),
    register_new_typesetter_printer(kTlpLiteral, NULL),
    register_new_typesetter_printer(kTlpSingleNote, NULL),
    register_new_typesetter_printer(kTlpNoteSep, NULL),
    register_new_typesetter_printer(kTlpSepBar, NULL),
    register_new_typesetter_printer(kTlpSavePoint, NULL),
    register_new_typesetter_printer(kTlpBend, NULL),
    register_new_typesetter_printer(kTlpReleaseBend, NULL),
    register_new_typesetter_printer(kTlpBlockEnd, NULL),
    register_new_typesetter_printer(kTlpTapping, NULL),
    register_new_typesetter_printer(kTlpNaturalHarmonic, NULL),
    register_new_typesetter_printer(kTlpArtificialHarmonic, NULL)
};

size_t g_txttypesetter_printer_nr = sizeof(g_txttypesetter_printers) / sizeof(g_txttypesetter_printers[0]);

static int g_txttypesetter_curr_row = 1;

int txttypesetter_get_curr_master_row_value() {
    return g_txttypesetter_curr_row;
}

int txttypesetter_inc_curr_master_row_value(const int amount) {
    g_txttypesetter_curr_row += amount;
    return txttypesetter_get_curr_master_row_value();
}

int txttypesetter_reset_curr_master_row_value() {
    g_txttypesetter_curr_row = 1;
}

txttypesetter_tablature_ctx *txttypesetter_get_properly_output_location(txttypesetter_tablature_ctx **tab, const int intended_consumption) {
    txttypesetter_tablature_ctx *tp = NULL;
    size_t *fretboard_size = NULL;
    if (tab == NULL) {
        return NULL;
    }
    if ((*tab) == NULL) {
        (*tab) = new_txttypesetter_tablature_ctx(tab);
        return (*tab);
    }
    fretboard_size = (size_t *) get_processor_setting("fretboard-size", NULL);
    if (fretboard_size == NULL) {
        return NULL;
    }
    if (txttypesetter_get_curr_master_row_value() + intended_consumption >= *fretboard_size) {
        tp = new_txttypesetter_tablature_ctx(tab);
        txttypesetter_reset_curr_master_row_value();
    } else {
        for (tp = (*tab); tp->next != NULL; tp = tp->next);
    }
    return tp;
}

int txttypesetter_eval_intended_buffer_consumption(const tulip_command_t techniques, const tulip_single_note_ctx *note, const txttypesetter_tablature_ctx *tab) {
    const tulip_single_note_ctx *np = NULL;
    txttypesetter_sustained_technique_ctx *tp = NULL;
    int longest = 0, l = 0;
    char *technique_label = NULL;
    tulip_command_t curr_technique = kTlpNone;

    if (techniques == kTlpNone) {
        return 0;
    }

    if ((curr_technique = (techniques & kTlpMute)) || (curr_technique = (techniques & kTlpLetRing)) ||
        (curr_technique = (techniques & kTlpBeat)) || (curr_technique = (techniques & kTlpTremoloPicking))) {
        technique_label = get_technique_label(curr_technique);
        l = 1;
        for (tp = tab->techniques; tp != NULL && l == 1; tp = tp->next) {
            if (strstr(tp->data, technique_label) == tp->data) {
                l++;
            }
        }
        //  WARN(Santiago): In practice it is uncommon things like let-ring + palm-mute anyway let's support other
        //                  weird combinations. As a result we will get ugly outputs like "tpbt......." :-P~~~
        return l + txttypesetter_eval_intended_buffer_consumption(techniques & ~(curr_technique), note, tab);
    } else if (techniques & kTlpChord) {
        longest = -1;
        for (np = note; np != NULL && (np->techniques & kTlpChord) == 0; np = np->next) {
            if (np->buf[0] == 0) {
                continue;
            }
            l = txttypesetter_eval_intended_buffer_consumption(kTlpSingleNote, np, tab);
            if (l > longest) {
                longest = l;
            }
        }
        return longest + txttypesetter_eval_intended_buffer_consumption(techniques & ~(kTlpChord | kTlpSingleNote), note, tab);
    } else if (techniques & kTlpSingleNote) {
        return strlen(note->buf) - 1;
    } else if ((techniques & kTlpVibrato) || (techniques & kTlpSlideDown)       ||
               (techniques & kTlpSlideUp) || (techniques & kTlpHammerOn)        ||
               (techniques & kTlpPullOff) || (techniques & kTlpVibratoWBar)     ||
               (techniques & kTlpNoteSep) || (techniques & kTlpSepBar)          ||
               (techniques & kTlpBend   ) || (techniques & kTlpReleaseBend)     ||
               (techniques & kTlpTapping) || (techniques & kTlpNaturalHarmonic) ||
               (techniques & kTlpArtificialHarmonic)) {
            return 1 + txttypesetter_eval_intended_buffer_consumption(techniques & ~(kTlpVibrato         |
                                                                                     kTlpSlideDown       |
                                                                                     kTlpSlideUp         |
                                                                                     kTlpHammerOn        |
                                                                                     kTlpPullOff         |
                                                                                     kTlpVibratoWBar     |
                                                                                     kTlpNoteSep         |
                                                                                     kTlpSepBar          |
                                                                                     kTlpBend            |
                                                                                     kTlpReleaseBend     |
                                                                                     kTlpTapping         |
                                                                                     kTlpNaturalHarmonic |
                                                                                     kTlpArtificialHarmonic), note, tab);
    }

    return 0;
}

int txt_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    const tulip_single_note_ctx *sp = NULL;
    txttypesetter_tablature_ctx *tab = NULL;
    txttypesetter_print_func print;
    tulip_command_t *dmuxd_cmds = NULL;
    size_t dmuxd_cmds_sz = 0;
    if (song == NULL || tabpath == NULL) {
        return 0;
    }
    for (sp = song; sp != NULL; sp = sp->next) {
        dmuxd_cmds = demux_tlp_commands(sp->techniques, &dmuxd_cmds_sz);
        while (dmuxd_cmds_sz-- > 0) {
            print = g_txttypesetter_printers[dmuxd_cmds[dmuxd_cmds_sz]];
            if (print != NULL) {
                print(&tab, sp);
            }
        }
    }
    return 1;
}
