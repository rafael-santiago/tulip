/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/ps/psinkspill.h>
#include <processor/typesetters/ps/psboundaries.h>
#include <stdio.h>

static int g_ps_cpage = 0;

struct pstypesetter_tab_diagram_ctx {
    int cxl, cxr;
    int cy, cx;
};

static struct pstypesetter_tab_diagram_ctx g_ps_ctab;

static void pstypesetter_init(void);

static FILE *pstypesetter_newps(const char *filepath);

static void pstypesetter_closeps(FILE *fp);

static void pstypesetter_newpage(FILE *fp);

static void pstypesetter_showpage(FILE *fp);

static void pstypesetter_newtabdiagram(FILE *fp, const int sn);

static void pstypesetter_proc_tabchunk(FILE *fp, const txttypesetter_tablature_ctx *tab);

static int pstypesetter_string_y(const int sn, const int cy);

static int pstypesetter_pinch_y(const int sn, const int cy);

static void pstypesetter_flush_fretboard_pinches(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void pstypesetter_init(void) {
    g_ps_cpage = 0;
}

static void pstypesetter_pageinit(void) {
    g_ps_ctab.cxl = PSTYPESETTER_PAGEXL;
    g_ps_ctab.cxr = PSTYPESETTER_PAGEXR;
    g_ps_ctab.cy = PSTYPESETTER_PAGEY;
}

static FILE *pstypesetter_newps(const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (fp == NULL) {
        return NULL;
    }
    fprintf(fp, "%%!PS-Adobe-3.0\n"
                "/Courier-Bold 11 selectfont\n");
    return fp;
}

static void pstypesetter_closeps(FILE *fp) {
    fprintf(fp, "showpage\n");
    fclose(fp);
}

static void pstypesetter_newpage(FILE *fp) {
    g_ps_cpage++;
    fprintf(fp, "%%%%Page: %d %d\n"
                "%%%%BeginPageSetup\n"
                "/pgsave save def\n"
                "%%%%IncludeResource: font TimesRoman\n"
                "%%%%EndPageSetup\n", g_ps_cpage, g_ps_cpage);
}

static void pstypesetter_showpage(FILE *fp) {
    fprintf(fp, "showpage\n");
}

static void pstypesetter_newtabdiagram(FILE *fp, const int sn) {
    int s = 0;
    int sy = 0;

    if (g_ps_cpage == 0) {
        pstypesetter_newpage(fp);
    }

    g_ps_ctab.cy += PSTYPESETTER_NEXT_TABCHUNK;

    for (s = 0; s < sn; s++) {
        if (pstypesetter_string_y(s, g_ps_ctab.cy) < PSTYPESETTER_PAGEY_LIMIT) {
            pstypesetter_pageinit();
            pstypesetter_showpage(fp);
            pstypesetter_newpage(fp);
            break;
        }
    }

    g_ps_ctab.cx = PSTYPESETTER_CARRIAGEX;

    for (s = 0; s < sn; s++) {
        sy = pstypesetter_string_y(s, g_ps_ctab.cy);
        fprintf(fp, "%d %d moveto\n"
                    "%d %d lineto\n", g_ps_ctab.cxl, sy, g_ps_ctab.cxr, sy);
    }
}

static int pstypesetter_string_y(const int sn, const int cy) {
    return (cy + (PSTYPESETTER_NEXT_STRING * sn));
}

static int pstypesetter_pinch_y(const int sn, const int cy) {
    return (pstypesetter_string_y(sn, cy) - PSTYPESETTER_ONTO_STRING_DELTA);
}

static void pstypesetter_proc_tabchunk(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    pstypesetter_newtabdiagram(fp, tab->string_nr);

    // TODO(Rafael): Handle sustained techniques, .times{n}, user quoting, etc.
    // (...)


    pstypesetter_flush_fretboard_pinches(fp, tab);
}

static void pstypesetter_flush_fretboard_pinches(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    size_t s, o;
    int x = g_ps_ctab.cx, py = 0;

    for (o = 0; o < tab->fretboard_sz; o++) {

        for (s = 0; s < tab->string_nr; s++) {

            switch (tab->strings[s][o]) {
                case 'h':
                case 'p':
                    py = pstypesetter_pinch_y(s, g_ps_ctab.cy);
                    // TODO(Rafael): Ligados. Rotacionar '('...
                    break;

                case '-':
                    break;

                case '|':
                    py = pstypesetter_pinch_y(s, g_ps_ctab.cy);
                    // TODO(Rafael): Vertical bar.
                    s = tab->string_nr + 1;
                    continue;

                default:
                    fprintf(fp, "%d %d moveto (%c) show\n", x,
                        pstypesetter_pinch_y(s, g_ps_ctab.cy),
                                           tab->strings[s][o]);
                    break;
            }

        }

        x += PSTYPESETTER_CARRIAGE_STEP;
    }

    g_ps_ctab.cx = PSTYPESETTER_CARRIAGEX;
}

int pstypesetter_inkspill(const char *filepath, const txttypesetter_tablature_ctx *tab, const tulip_single_note_ctx *song) {
    FILE *fp = NULL;
    const txttypesetter_tablature_ctx *tp = NULL;

    if (filepath == NULL || tab == NULL) {
        return 0;
    }

    if ((fp = pstypesetter_newps(filepath)) == NULL) {
        return 0;
    }

    pstypesetter_init();

    pstypesetter_pageinit();

    for (tp = tab; tp != NULL; tp = tp->next) {
        pstypesetter_proc_tabchunk(fp, tp);
    }

    pstypesetter_closeps(fp);

    return 1;
}
