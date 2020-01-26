/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/svg/svg.h>
#include <processor/typesetters/svg/svgboundaries.h>
#include <processor/typesetters/txt/txtctx.h>
#include <processor/typesetters/txt/txt.h>
#include <processor/typesetters/svg/svginkspill.h>
#include <processor/typesetters/typeprefs.h>
#include <processor/settings.h>
#include <usrland/cmdlineoptions.h>
#include <stdio.h>

int svg_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    txttypesetter_tablature_ctx *tab = NULL;
    int has_error = 1;
    size_t fretboard_sz = SVGTYPESETTER_FRETBOARD_SIZE;
    extern int g_svgtypesetter_page_width, g_svgtypesetter_page_height;
    extern char *g_svgtypesetter_svg_encoding;

    if (typesetter_paper_size(&g_svgtypesetter_page_width, &g_svgtypesetter_page_height) == 0) {
        fprintf(stderr, "processor ERROR: Invalid paper size.\n");
        return 0;
    }

    g_svgtypesetter_svg_encoding = (char *)get_option("svg-encoding", SVGTYPESETTER_DEFAULT_SVG_ENCODING);

    set_processor_setting("fretboard-size", &fretboard_sz, sizeof(fretboard_sz));

    if (song == NULL || tabpath == NULL) {
        return 0;
    }

    if ((tab = txt_typesetter_gettab(song)) == NULL) {
        return 0;
    }

    apply_final_output_brush_up(tab);

    has_error = (svgtypesetter_inkspill(tabpath, tab, song) == 0);

    free_txttypesetter_tablature_ctx(tab);

    return has_error;
}
