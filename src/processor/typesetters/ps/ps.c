/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/ps/ps.h>
#include <processor/typesetters/txt/txtctx.h>
#include <processor/typesetters/txt/txt.h>
#include <processor/typesetters/ps/psinkspill.h>
#include <processor/settings.h>

int ps_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    txttypesetter_tablature_ctx *tab = NULL;
    int has_error = 1;

    if (song == NULL || tabpath == NULL) {
        return 0;
    }

    if ((tab = txt_typesetter_gettab(song)) == NULL) {
        return 0;
    }

    apply_final_output_brush_up(tab);

    has_error = (pstypesetter_inkspill(tabpath, tab, song) == 0);

    free_txttypesetter_tablature_ctx(tab);

    return has_error;
}
