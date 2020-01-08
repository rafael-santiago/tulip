/*
 *                           Copyright (C) 2005-2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/svg/svginkspill.h>
#include <processor/typesetters/svg/svgboundaries.h>
#include <string.h>
#include <stdio.h>

struct svgtypesetter_tab_diagram_ctx {
    int xlim_left, xlim_right;
    int carriage_x, carriage_y;
};

struct svgtypesetter_page_ctx {
    FILE *fp;
    int y;
    int page_nr;
    char filename[4096], curr_pagefile[4096];
    struct svgtypesetter_tab_diagram_ctx tab;
};

static int svgtypesetter_init(const char *filename);

static int svgtypesetter_newpage(void);

static void svgtypesetter_fclose(void);

static void svgtypesetter_spill_tabdiagram(void);

static void svgtypesetter_spill_comments(const txttypesetter_tablature_ctx *txttab);

static void svgtypesetter_spill_sustained_techniques(const txttypesetter_tablature_ctx *txttab);

static void svgtypesetter_spill_times(const txttypesetter_tablature_ctx *txttab);

static void svgtypesetter_flush_fretboard_pinches(const txttypesetter_tablature_ctx *txttab);

static struct svgtypesetter_page_ctx g_svg_page;

static void svgtypesetter_spill_comments(const txttypesetter_tablature_ctx *txttab) {
    // TODO(Rafael): Guess what?
}

static void svgtypesetter_spill_sustained_techniques(const txttypesetter_tablature_ctx *txttab) {
    // TODO(Rafael): Guess what?
}

static void svgtypesetter_spill_times(const txttypesetter_tablature_ctx *txttab) {
    // TODO(Rafael): Guess what?
}

static void svgtypesetter_flush_fretboard_pinches(const txttypesetter_tablature_ctx *txttab) {
    // TODO(Rafael): Guess what?
}

static int svgtypesetter_newpage(void) {
    snprintf(g_svg_page.curr_pagefile, sizeof(g_svg_page.curr_pagefile) - 1, "%s-%d-pp.svg", g_svg_page.filename,
                                                                                             g_svg_page.page_nr);

    if (g_svg_page.fp != NULL) {
        svgtypesetter_fclose();
    }

    if ((g_svg_page.fp = fopen(g_svg_page.curr_pagefile, "w")) == NULL) {
        return 0;
    }

    g_svg_page.tab.xlim_left = SVGTYPESETTER_TAB_XL_DELTA;
    g_svg_page.tab.xlim_right = SVGTYPESETTER_PAGE_WIDTH + SVGTYPESETTER_TAB_XR_DELTA;

    g_svg_page.tab.carriage_x = g_svg_page.tab.xlim_left;
    g_svg_page.tab.carriage_y = SVGTYPESETTER_TAB_Y;

    fprintf(g_svg_page.fp, "<svg width=\"%d\" height=\"%d\">\n", SVGTYPESETTER_PAGE_WIDTH, SVGTYPESETTER_PAGE_HEIGHT);
    fprintf(g_svg_page.fp, "\t<defs>\n"
                            "\t\t<marker id=\"arrow\" markerWidth=\"10\" markerHeight=\"10\" refX=\"1\" refY=\"4\""
                            " orient=\"auto\" markerUnits=\"strokeWidth\">"
                            "\t\t\t<path d=\"M0,0 L0,8 L9,3 z\" fill=\"black\"/>"
                            "\t\t</marker>"
                            "\t</defs>");
    return 1;
}

static void svgtypesetter_spill_tabdiagram(void) {
    int x, y, s;

    x = g_svg_page.tab.xlim_left;
    y = g_svg_page.tab.carriage_y;

    // INFO(Rafael): More left vertical 'TAB' indication.
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" fill=\"black\" font-size=\"15\" font-weight=\"bold\">T</text>\n"
                           "\t<text x=\"%d\" y=\"%d\" fill=\"black\" font-size=\"15\" font-weight=\"bold\">A</text>\n"
                           "\t<text x=\"%d\" y=\"%d\" fill=\"black\" font-size=\"15\" font-weight=\"bold\">B</text>\n",
                           x, y + SVGTYPESETTER_TAB_Y_SPAN + 5,
                           x, y + SVGTYPESETTER_TAB_Y_SPAN + 5 + 18,
                           x, y + SVGTYPESETTER_TAB_Y_SPAN + 5 + 35);


    // INFO(Rafael): Vertical left line.
    fprintf(g_svg_page.fp, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\""
                           " style=\"stroke:rgb(0,0,0);stroke-width:1;opacity:0.5\"/>", x, x, y,
                                                                                        y + SVGTYPESETTER_TAB_Y_SPAN * 10);
    // INFO(Rafael): String lines.
    for (s = 0; s < 6; s++) {
        fprintf(g_svg_page.fp, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\""
                               " style=\"stroke:rgb(0,0,0);stroke-width:1;opacity:0.5\"/>", x, g_svg_page.tab.xlim_right,
                                                                                            y, y);
        y += SVGTYPESETTER_TAB_Y_SPAN;
    }

    // INFO(Rafael): Vertical right line.
    fprintf(g_svg_page.fp, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\""
                           " style=\"stroke:rgb(0,0,0);stroke-width:1;opacity:0.5\"/>", g_svg_page.tab.xlim_right,
                                                                                        g_svg_page.tab.xlim_right,
                                                                                        g_svg_page.tab.carriage_y,
                                                                                        g_svg_page.tab.carriage_y +
                                                                                            SVGTYPESETTER_TAB_Y_SPAN * 10);
}

static void svgtypesetter_fclose(void) {
    if (g_svg_page.fp != NULL) {
        // TODO(Rafael): Spill page number at end of the page.
        fprintf(g_svg_page.fp, "</svg>\n");
        fclose(g_svg_page.fp);
        g_svg_page.fp = NULL;
        g_svg_page.page_nr++;
    }
}

static int svgtypesetter_init(const char *filename) {
    char *p;

    if (filename == NULL) {
        return 0;
    }

    snprintf(g_svg_page.filename, sizeof(g_svg_page.filename) - 1, "%s", filename);

    if ((p = strstr(g_svg_page.filename, ".svg")) != NULL) {
        *p = 0;
    }

    g_svg_page.page_nr = 1;

    g_svg_page.fp = NULL;

    return 1;
}

int svgtypesetters_svginkspill(const char *filepath, const txttypesetter_tablature_ctx *tab,
                               const tulip_single_note_ctx *song) {
    return 0;
}

