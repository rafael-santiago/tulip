/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/txt.h>
#include <processor/typesetters/txt/txttypes.h>
#include <dsl/utils.h>
#include <stdlib.h>

typedef void (*txttypesetter_print_func)(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note);

struct txttypesetter_printer {
    txttypesetter_print_func print;
};

#define register_new_typesetter_printer(t, p) { (p) } //  INFO(Santiago): "t" it is just for documenting issues.

static struct txttypesetter_printer g_txttypesetter_printers[] = {
    register_new_typesetter_printer(kTlpMute, NULL),
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

int txt_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    const tulip_single_note_ctx *sp = NULL;
    txttypesetter_tablature_ctx *tab = NULL;
    txttypesetter_print_func print;
    tulip_command_t *dmuxd_cmds = NULL;
    size_t dmuxd_cmds_sz = 0;
    for (sp = song; sp != NULL; sp = sp->next) {

    }
    return 0;
}
