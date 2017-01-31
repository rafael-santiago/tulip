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
    int cy;
};

static struct pstypesetter_tab_diagram_ctx g_ps_ctab;

static void pstypesetter_init(void);

static FILE *pstypesetter_newps(const char *filepath);

static void pstypesetter_closeps(FILE *fp);

static void pstypesetter_newpage(FILE *fp);

static void pstypesetter_showpage(FILE *fp);

static void pstypesetter_newtabdiagram(FILE *fp);

static void pstypesetter_proc_tabchunk(const txttypesetter_tablature_ctx *tab);

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
    fprintf(fp, "%%!PS-Adobe-3.0\n");
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

static void pstypesetter_newtabdiagram(FILE *fp) {
    int s = 0;

    if (g_ps_cpage == 0) {
        pstypesetter_newpage(fp);
    } else {
        pstypesetter_showpage(fp);
    }

    g_ps_ctab.cy += PSTYPESETTER_NEXT_TABCHUNK;

    for (s = 0; s < 6; s++) {
        fprintf(fp, "%d %d moveto\n"
                    "%d %d lineto\n", g_ps_ctab.cxl, g_ps_ctab.cy, g_ps_ctab.cxr, g_ps_ctab.cy);
        g_ps_ctab.cy += PSTYPESETTER_NEXT_STRING;
    }
}

static void pstypesetter_proc_tabchunk(const txttypesetter_tablature_ctx *tab) {
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
        pstypesetter_proc_tabchunk(tp);
    }

    pstypesetter_closeps(fp);

    return 1;
}
