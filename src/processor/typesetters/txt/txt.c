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
#include <processor/typesetters/txt/printers/letringtxtprinter.h>
#include <processor/typesetters/txt/printers/strumtxtprinter.h>
#include <processor/typesetters/txt/printers/tremolopickingtxtprinter.h>
#include <processor/typesetters/txt/printers/vibratowbartxtprinter.h>
//#include <processor/typesetters/txt/printers/chordtxtprinter.h>
#include <processor/typesetters/txt/printers/vibratotxtprinter.h>
#include <processor/typesetters/txt/printers/slidedowntxtprinter.h>
#include <processor/typesetters/txt/printers/slideuptxtprinter.h>
#include <processor/typesetters/txt/printers/noteseptxtprinter.h>
#include <processor/typesetters/txt/printers/sepbartxtprinter.h>
#include <processor/typesetters/txt/printers/bendtxtprinter.h>
#include <processor/typesetters/txt/printers/releasebendtxtprinter.h>
#include <processor/typesetters/txt/printers/tappingtxtprinter.h>
#include <processor/typesetters/txt/printers/naturalharmonictxtprinter.h>
#include <processor/typesetters/txt/printers/artificialharmonictxtprinter.h>
#include <processor/typesetters/txt/printers/singlenotetxtprinter.h>
#include <processor/typesetters/txt/printers/savepointtxtprinter.h>
#include <processor/typesetters/txt/printers/hammerontxtprinter.h>
#include <processor/typesetters/txt/printers/pullofftxtprinter.h>
#include <processor/typesetters/txt/printers/tunningtxtprinter.h>
#include <processor/typesetters/txt/printers/literaltxtprinter.h>
#include <processor/typesetters/txt/txtinkspill.h>
#include <processor/settings.h>
#include <processor/oututils.h>
#include <dsl/utils.h>
#include <stdlib.h>
#include <string.h>

//  WARN(Santiago): For this typesetter I am taking in consideration the usage of monospaced fonts.
//                  If you like to use non-monospaced fonts for your plain-text guitar tablatures -> step off <-

typedef void (*txttypesetter_print_func)(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note);

#define register_new_typesetter_printer(tc, p) (p) //  INFO(Santiago): Hello sloppy creatures, "tc" is just for documenting issues.

static void txttypesetter_blockend_handler(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note);

static void txttypesetter_chord_handler(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note);

static txttypesetter_print_func g_txttypesetter_printers[] = {
    register_new_typesetter_printer(kTlpSavePoint, txttypesetter_savepoint_printer),
    register_new_typesetter_printer(kTlpNoteSep, txttypesetter_notesep_printer),
    register_new_typesetter_printer(kTlpBlockEnd, txttypesetter_blockend_handler),
    register_new_typesetter_printer(kTlpMute, txttypesetter_mute_printer),
    register_new_typesetter_printer(kTlpLetRing, txttypesetter_letring_printer),
    register_new_typesetter_printer(kTlpChord, txttypesetter_chord_handler),
    register_new_typesetter_printer(kTlpStrum, txttypesetter_strum_printer),
    register_new_typesetter_printer(kTlpTremoloPicking, txttypesetter_tremolopicking_printer),
    register_new_typesetter_printer(kTlpVibratoWBar, txttypesetter_vibratowbar_printer),
    register_new_typesetter_printer(kTlpVibrato, txttypesetter_vibrato_printer),
    register_new_typesetter_printer(kTlpSlideDown, txttypesetter_slidedown_printer),
    register_new_typesetter_printer(kTlpSlideUp, txttypesetter_slideup_printer),
    register_new_typesetter_printer(kTlpHammerOn, txttypesetter_hammeron_printer),
    register_new_typesetter_printer(kTlpPullOff, txttypesetter_pulloff_printer),
    register_new_typesetter_printer(kTlpTunning, txttypesetter_tunning_printer),
    register_new_typesetter_printer(kTlpLiteral, txttypesetter_literal_printer),
    register_new_typesetter_printer(kTlpSingleNote, txttypesetter_singlenote_printer),
    register_new_typesetter_printer(kTlpSepBar, txttypesetter_sepbar_printer),
    register_new_typesetter_printer(kTlpBend, txttypesetter_bend_printer),
    register_new_typesetter_printer(kTlpReleaseBend, txttypesetter_releasebend_printer),
    register_new_typesetter_printer(kTlpTapping, txttypesetter_tapping_printer),
    register_new_typesetter_printer(kTlpNaturalHarmonic, txttypesetter_naturalharmonic_printer),
    register_new_typesetter_printer(kTlpArtificialHarmonic, txttypesetter_artificialharmonic_printer)
};

const size_t g_txttypesetter_printer_nr = sizeof(g_txttypesetter_printers) / sizeof(g_txttypesetter_printers[0]);

static int g_txttypesetter_has_active_chord = 0;

static void txttypesetter_blockend_handler(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note) {

    if (tab == NULL || (*tab)->active_techniques == NULL) {
        return;
    }

    if (g_txttypesetter_has_active_chord) {
        g_txttypesetter_has_active_chord = 0;
        return;
    }

    (*tab)->active_techniques = pop_technique_from_txttypesetter_active_technique_ctx((*tab)->active_techniques);

}

static void txttypesetter_chord_handler(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note) {
    if (tab == NULL || g_txttypesetter_has_active_chord) {
        return;
    }

    g_txttypesetter_has_active_chord = 1;
}

void txttypesetter_print_sustained_technique_mark(const tulip_command_t command, txttypesetter_tablature_ctx **tab, const row_usage) {
    txttypesetter_active_technique_ctx *ap = NULL;

    if (tab == NULL) {
        return;
    }

    for (ap = (*tab)->active_techniques; ap != NULL && ap->technique != command; ap = ap->next);

    if (ap != NULL) {
        sustain_active_techniques((*tab)->active_techniques, row_usage, (*tab)->curr_row);
        return;
    }

    (*tab)->active_techniques = push_technique_to_txttypesetter_active_technique_ctx((*tab)->active_techniques, command, &(*tab)->techniques, &(*tab)->curr_row);
}

txttypesetter_tablature_ctx *txttypesetter_get_properly_output_location(txttypesetter_tablature_ctx **tab, const int row_usage) {
    txttypesetter_tablature_ctx *tp = NULL;

    if (tab == NULL) {
        return NULL;
    }

    if ((*tab) == NULL) {
        (*tab) = new_txttypesetter_tablature_ctx(tab);
        return (*tab);
    }

    if (((*tab)->curr_row + row_usage) >= (*tab)->fretboard_sz) {
        tp = new_txttypesetter_tablature_ctx(tab);
    } else {
        for (tp = (*tab); tp->next != NULL; tp = tp->next);
    }

    return tp;
}

int txttypesetter_eval_buffer_row_usage(const tulip_command_t techniques, const tulip_single_note_ctx *note, const txttypesetter_tablature_ctx *tab) {
    const tulip_single_note_ctx *np = NULL;
    txttypesetter_sustained_technique_ctx *tp = NULL;
    int longest = 0, l = 0;
    char *technique_label = NULL;
    tulip_command_t curr_technique = kTlpNone;

    if (techniques == kTlpNone) {
        return 0;
    }

    if ((curr_technique = (techniques & kTlpMute)) || (curr_technique = (techniques & kTlpLetRing)) ||
        (curr_technique = (techniques & kTlpStrum)) || (curr_technique = (techniques & kTlpTremoloPicking))) {
        technique_label = get_technique_label(curr_technique);
        l = 1;
        if (tab != NULL) {
            for (tp = tab->techniques; tp != NULL && l == 1; tp = tp->next) {
                if (strstr(tp->data, technique_label) == tp->data) {
                    l++;
                }
            }
        }
        //  WARN(Santiago): In practice is uncommon things like let-ring + palm-mute anyway let's support other
        //                  weird combinations. As a result we will get ugly outputs like: "tp.......
        //                                                                                  st......."...
        return l + txttypesetter_eval_buffer_row_usage(techniques & ~(curr_technique), note, tab);
    } else if (techniques & kTlpChord) {
        longest = -1;
        for (np = note; np != NULL && (np->techniques & kTlpChord) == 0; np = np->next) {
            if (np->buf[0] == 0) {
                continue;
            }
            l = txttypesetter_eval_buffer_row_usage(kTlpSingleNote, np, tab);
            if (l > longest) {
                longest = l;
            }
        }
        return longest + txttypesetter_eval_buffer_row_usage(techniques & ~(kTlpChord | kTlpSingleNote), note, tab);
    } else if (techniques & kTlpSingleNote) {
        return strlen(note->buf) - 1 + txttypesetter_eval_buffer_row_usage(techniques & ~(kTlpSingleNote), note, tab);
    } else if ((techniques & kTlpVibrato) || (techniques & kTlpSlideDown)       ||
               (techniques & kTlpSlideUp) || (techniques & kTlpHammerOn)        ||
               (techniques & kTlpPullOff) || (techniques & kTlpVibratoWBar)     ||
               (techniques & kTlpNoteSep) || (techniques & kTlpSepBar)          ||
               (techniques & kTlpBend   ) || (techniques & kTlpReleaseBend)     ||
               (techniques & kTlpTapping) || (techniques & kTlpNaturalHarmonic) ||
               (techniques & kTlpArtificialHarmonic)) {
            return 1 + txttypesetter_eval_buffer_row_usage(techniques & ~(kTlpVibrato         |
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

static int show_curr_fretboard(const txttypesetter_tablature_ctx *tab) {
    const txttypesetter_tablature_ctx *tp = tab;
    size_t s = 0;
    if (tp == NULL) {
        return;
    }
    while (tp->next != NULL) {
        tp = tp->next;
    }
    txttypesetter_sustained_technique_ctx *sp = NULL;
    for (sp = tp->techniques; sp != NULL; sp = sp->next) {
        printf("%s\n", sp->data);
    }
    for (s = 0; s < 6; s++) {
        printf("%s\n", tp->strings[s]);
    }
    system("read");
}

int txt_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    const tulip_single_note_ctx *sp = NULL;
    txttypesetter_tablature_ctx *tab = NULL, *tp = NULL;
    txttypesetter_print_func print;
    tulip_command_t *demuxes = NULL;
    size_t demuxes_nr = 0;
    int has_error = 1;

    if (song == NULL || tabpath == NULL) {
        return 0;
    }

    tp = tab;

    for (sp = song; sp != NULL; sp = sp->next) {
        demuxes = demux_tlp_commands(sp->techniques, &demuxes_nr);

        if (demuxes == NULL) {
            continue;
        }

        while (demuxes_nr-- > 0) {
            //printf("(index: %d) (cmd: %x) (note: %s)\n", tlp_cmd_code_to_plain_index(demuxes[demuxes_nr]), demuxes[demuxes_nr], sp->buf);
            /*if (tab != NULL) {
                printf("(%d) %.8x / mux: %.8x / note %s %d\n", tlp_cmd_code_to_plain_index(demuxes[demuxes_nr]), sp->techniques, demuxes[demuxes_nr], sp->buf, tab->curr_row);
            }*/
            print = g_txttypesetter_printers[tlp_cmd_code_to_plain_index(demuxes[demuxes_nr])];
            if (print != NULL) {
                print(&tp, sp);
            }
            //show_curr_fretboard(tp);
        }

        free(demuxes);

        if (tp == NULL) {
            continue;
        }

        if (tab == NULL && tp != NULL) {
            tab = tp;
        }

        while (tp->next != NULL) {
            tp = tp->next;
        }
    }

    if (tab == NULL) {
        return 1;
    }

    has_error = (txttypesetter_inkspill(tabpath, tab) != 1);

    free_txttypesetter_tablature_ctx(tab);

    return has_error;
}
