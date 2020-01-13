/*
 *                           Copyright (C) 2005-2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/svg/svginkspill.h>
#include <processor/typesetters/svg/svgboundaries.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

struct svgtypesetter_pinch_point_ctx {
    int x, y;
};

struct svgtypesetter_tab_diagram_ctx {
    int xlim_left, xlim_right;
    int *carriage_x, *carriage_y;
    struct svgtypesetter_pinch_point_ctx fbrd[6];
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

static void svgtypesetter_spill_song_title(const char *title);

static void svgtypesetter_spill_transcriber(const char *name);

static void svgtypesetter_spill_comments(const txttypesetter_tablature_ctx *txttab);

static void svgtypesetter_spill_sustained_techniques(const txttypesetter_tablature_ctx *txttab);

static void svgtypesetter_spill_times(const txttypesetter_tablature_ctx *txttab);

static void svgtypesetter_flush_fretboard_pinches(const txttypesetter_tablature_ctx *txttab);

static void svgtypesetter_flush_hammer_on_pull_off_pinch(void);

static void svgtypesetter_flush_bend_pinch(const int arrow);

static void svgtypesetter_flush_release_bend_pinch(const int arrow);

static void svgtypesetter_flush_note_pinch(const char *note);

static void svgtypesetter_flush_vibrato_pinch(void);

static void svgtypesetter_flush_slide_down_pinch(void);

static void svgtypesetter_flush_slide_up_pinch(void);

static void svgtypesetter_insert_header_span(void);

static int svgtypesetter_refresh_fbrd_xy(void);

static inline void svgtypesetter_note_sep_xstep(void);

static inline void svgtypesetter_bend_xstep(void);

static inline void svgtypesetter_release_bend_xstep(void);

static inline void svgtypesetter_hammer_on_xstep(void);

static inline void svgtypesetter_pull_off_xstep(void);

static inline void svgtypesetter_vibrato_xstep(void);

static inline void svgtypesetter_slide_down_xstep(void);

static inline void svgtypesetter_slide_up_xstep(void);

static int svgtypesetter_is_chord(const char **strings, const size_t offset);

static size_t svgtypesetter_get_release_bend_arrow_string(const char **strings, const size_t offset);

static size_t svgtypesetter_get_bend_arrow_string(const char **strings, const size_t offset);

static struct svgtypesetter_page_ctx g_svg_page;

static inline void svgtypesetter_hammer_on_xstep(void) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_TAB_X_SPAN + 10;
}

static inline void svgtypesetter_vibrato_xstep(void) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_TAB_X_SPAN;
}

static inline void svgtypesetter_pull_off_xstep(void) {
    svgtypesetter_hammer_on_xstep();
}

static inline void svgtypesetter_release_bend_xstep(void) {
    svgtypesetter_bend_xstep();
}

static inline void svgtypesetter_bend_xstep(void) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_TAB_X_SPAN + 10;
}

static inline void svgtypesetter_note_sep_xstep(void) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_TAB_X_SPAN;
}

static inline void svgtypesetter_slide_down_xstep(void) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_TAB_X_SPAN - 10;
}

static inline void svgtypesetter_slide_up_xstep(void) {
    svgtypesetter_slide_down_xstep();
}

static void svgtypesetter_insert_header_span(void) {
    *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN * 3;
    svgtypesetter_refresh_fbrd_xy();
}

static void svgtypesetter_spill_song_title(const char *title) {
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" fill=\"black\" font-size=\"30\" font-weight=\"bold\">%s"
                           "</text>\n", *g_svg_page.tab.carriage_x, *g_svg_page.tab.carriage_y, title);
    *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN * 2;
    svgtypesetter_refresh_fbrd_xy();
}

static void svgtypesetter_spill_transcriber(const char *name) {
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" fill=\"black\" font-size=\"13\""
                           " font-style=\"italic\" font-weight=\"bold\">%s</text>\n", *g_svg_page.tab.carriage_x,
                                                                                      *g_svg_page.tab.carriage_y, name);
    *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN * 2;
    svgtypesetter_refresh_fbrd_xy();
}

static void svgtypesetter_spill_comments(const txttypesetter_tablature_ctx *txttab) {
    txttypesetter_comment_ctx *cp;

    for (cp = txttab->comments; cp != NULL; cp = cp->next) {
        fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" fill=\"black\" font-size=\"18\">"
                               "%s</text>\n", *g_svg_page.tab.carriage_x,
                                              *g_svg_page.tab.carriage_y,
                                              cp->data);
        *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN * 2;
    }

    svgtypesetter_refresh_fbrd_xy();
}

static void svgtypesetter_flush_note_pinch(const char *note) {
    const char *np = note;

    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"13\" font-weight=\"bold\">", *g_svg_page.tab.carriage_x,
                                                                                               *g_svg_page.tab.carriage_y);
    while (isdigit(*np)) {
        fprintf(g_svg_page.fp, "%c", *np);
        np++;
    }

    fprintf(g_svg_page.fp, "</text>\n");
}

static void svgtypesetter_flush_bend_pinch(const int arrow) {
    char *arrow_markup = (arrow) ? " marker-end=\"(url#arrow)\"" : "";
    fprintf(g_svg_page.fp, "\t<path d=\"M%d,%d C%d,%d, %d,%d %d,%d\""
                           " fill=\"none\" stroke=\"black\" stroke-width=\"1\"%s/>", *g_svg_page.tab.carriage_x,
                                                                                     *g_svg_page.tab.carriage_y,
                                                                                     *g_svg_page.tab.carriage_x,
                                                                                     *g_svg_page.tab.carriage_y + 5,
                                                                                     *g_svg_page.tab.carriage_x + 5,
                                                                                     *g_svg_page.tab.carriage_y + 5,
                                                                                     *g_svg_page.tab.carriage_x + 5,
                                                                                     *g_svg_page.tab.carriage_y - 15,
                                                                                     arrow_markup);
}

static void svgtypesetter_flush_release_bend_pinch(const int arrow) {
    char *arrow_markup = (arrow) ? "marker-end=\"(url#arrow)\"" : "";
    fprintf(g_svg_page.fp, "\t<path d=\"M%d,%d C%d,%d %d,%d %d,%d\""
                           " fill=\"none\" stroke=\"black\" stroke-width=\"1\"%s/>", *g_svg_page.tab.carriage_x,
                                                                                     *g_svg_page.tab.carriage_y,
                                                                                     *g_svg_page.tab.carriage_x,
                                                                                     *g_svg_page.tab.carriage_y - 10,
                                                                                     *g_svg_page.tab.carriage_x + 5,
                                                                                     *g_svg_page.tab.carriage_y - 10,
                                                                                     *g_svg_page.tab.carriage_x + 4,
                                                                                     *g_svg_page.tab.carriage_y + 5,
                                                                                     arrow_markup);
}

static void svgtypesetter_flush_vibrato_pinch(void) {
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"18\">~</text>\n", *g_svg_page.tab.carriage_x,
                                                                                    *g_svg_page.tab.carriage_y);
}

static void svgtypesetter_flush_hammer_on_pull_off_pinch(void) {
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"18\""
                           " transform=\"rotate(90,162,65)\">(</text>\n", *g_svg_page.tab.carriage_x,
                                                                          *g_svg_page.tab.carriage_y);
}

static void svgtypesetter_flush_slide_down_pinch(void) {
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"18\">/</text>\n", *g_svg_page.tab.carriage_x,
                                                                                    *g_svg_page.tab.carriage_y);
}

static void svgtypesetter_flush_slide_up_pinch(void) {
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"18\">\\</text>\n", *g_svg_page.tab.carriage_x,
                                                                                    *g_svg_page.tab.carriage_y);
}

static int svgtypesetter_is_chord(const char **strings, const size_t offset) {
    size_t s;
    int is = 0;

    if (strings == NULL || *strings == 0) {
        return 0;
    }

    for (s = 0; s < 6 && is < 2; s++) {
        is += (strings[s][offset] != '-');
    }

    return (is == 2);
}

static size_t svgtypesetter_get_release_bend_arrow_string(const char **strings, const size_t offset) {
    ssize_t s;

    for (s = 5; s >= 0; s--) {
        if (strings[s][offset] == 'r') {
            return (size_t)s;
        }
    }

    return 6;
}

static size_t svgtypesetter_get_bend_arrow_string(const char **strings, const size_t offset) {
    size_t s;

    for (s = 0; s < 6; s++) {
        if (strings[s][offset] == 'b') {
            return s;
        }
    }

    return 6;
}

static void do_xstep(int *x, const char **strings, const size_t offset) {
    size_t s, x_span = 0;

    for (s = 0; s < 6; s++) {
        switch (strings[s][offset]) {
            case '-':
                if (x_span < SVGTYPESETTER_TAB_X_SPAN) {
                    x_span = SVGTYPESETTER_TAB_X_SPAN;
                }
                break;

            case 'h':
            case 'p':
                if (x_span < SVGTYPESETTER_TAB_X_SPAN + 10) {
                    x_span = SVGTYPESETTER_TAB_X_SPAN + 10;
                }
                break;

            case '/':
            case '\\':
                if (x_span < SVGTYPESETTER_TAB_X_SPAN - 10) {
                    x_span = SVGTYPESETTER_TAB_X_SPAN - 10;
                }
                break;

            case '~':
                if (x_span < SVGTYPESETTER_TAB_X_SPAN) {
                    x_span = SVGTYPESETTER_TAB_X_SPAN;
                }
                break;

            case 'b':
            case 'r':
                if (x_span < SVGTYPESETTER_TAB_X_SPAN + 10) {
                    x_span = SVGTYPESETTER_TAB_X_SPAN + 10;
                }
                break;

            default:
                if (isdigit(strings[s][offset]) && x_span < SVGTYPESETTER_TAB_X_SPAN) {
                    x_span = SVGTYPESETTER_TAB_X_SPAN;
                }
                break;
        }
    }

    *x += x_span;
}

static void svgtypesetter_spill_sustained_techniques(const txttypesetter_tablature_ctx *txttab) {
    const txttypesetter_sustained_technique_ctx *sp;
    int x[2], y, n = 0;
    size_t i;
    char *p, *p_end, *tp = NULL;
    size_t offset;

    for (sp = txttab->techniques; sp != NULL; sp = sp->next) {
        n += 1;
    }

    if (n > 0) {
        y = g_svg_page.tab.fbrd[0].y - SVGTYPESETTER_TAB_Y_SPAN * n;
        for (sp = txttab->techniques; sp != NULL; sp = sp->next) {
            i = 0;
            p = sp->data;
            p_end = p + strlen(p);
            x[0] = x[1] = g_svg_page.tab.xlim_left;
            offset = 0;
            while (p != p_end && i < 2 && offset < txttab->fretboard_sz) {
                do_xstep(&x[i], (const char **)txttab->strings, offset);
                i += (i == 0) ? isalpha(*p) : (*p != '.');
                if (isalpha(*p)) {
                    tp = p;
                    p += 1;
                }
                p++;
                offset++;
            }
            fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                   " font-size=\"13\" font-weight=\"bold\">%c%c%c</text>\n", x[0], y, tp[0], tp[1], tp[2]);
            fprintf(g_svg_page.fp, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\""
                                   " style=\"stroke:rgb(0,0,0);stroke-width:1;opacity:0.5\""
                                   " stroke-dasharray=\"5,5\"/>\n", x[0], x[1], y + 1, y + 1);
            y += SVGTYPESETTER_TAB_Y_SPAN;
        }
    }
}

static void svgtypesetter_spill_times(const txttypesetter_tablature_ctx *txttab) {
    int x;
    char *tp, *tp_end, *temp;
    size_t offset;
    char tm_buf[20];

    if (txttab->times == NULL) {
        return;
    }

    tp = txttab->times;
    tp_end = tp + strlen(tp);

    x = g_svg_page.tab.xlim_left;
    offset = 0;
    while (tp < tp_end && offset < txttab->fretboard_sz) {
        do_xstep(&x, (const char **)txttab->strings, offset);
        if (isdigit(*tp)) {
            if ((temp = strstr(tp, "X")) != NULL) {
                // INFO(Rafael): Under normal conditions 'X' must always be found.
                memset(tm_buf, 0, sizeof(tm_buf));
                memcpy(tm_buf, tp, temp - tp);
                fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                       " font-size=\"13\" font-weight=\"bold\">%s</text>\n", x,
                                                                                             g_svg_page.tab.fbrd[0].y -
                                                                                                SVGTYPESETTER_TAB_Y_SPAN,
                                                                                             tm_buf);
            }
            tp = temp + 1;
        } else {
            tp++;
        }
        offset++;
    }
}

static void svgtypesetter_flush_fretboard_pinches(const txttypesetter_tablature_ctx *txttab) {
    const txttypesetter_tablature_ctx *tp;
    size_t s, offset;
    void (*xstep)(void);
    int is_chord, bend_arrow_string;

#define do_flush_pinch(xstep, s, sn, arrow_string) {\
    if (*(s) == '~') {\
        svgtypesetter_flush_vibrato_pinch();\
        if (xstep == NULL || xstep == svgtypesetter_note_sep_xstep) {\
            xstep = svgtypesetter_vibrato_xstep;\
        }\
    } else if (*(s) == 'b') {\
        svgtypesetter_flush_bend_pinch(sn == arrow_string);\
        if (xstep == NULL || xstep == svgtypesetter_note_sep_xstep) {\
            xstep = svgtypesetter_bend_xstep;\
        }\
    } else if (*(s) == 'r') {\
        svgtypesetter_flush_release_bend_pinch(sn == arrow_string);\
        if (xstep == NULL || xstep == svgtypesetter_note_sep_xstep) {\
            xstep = svgtypesetter_release_bend_xstep;\
        }\
    } else if (*(s) == 'h' || *(s) == 'p') {\
        svgtypesetter_flush_hammer_on_pull_off_pinch();\
        if (xstep == NULL || xstep == svgtypesetter_note_sep_xstep) {\
            xstep = svgtypesetter_hammer_on_xstep;\
        }\
    } else if (*(s) == '/') {\
        svgtypesetter_flush_slide_down_pinch();\
        if (xstep == NULL || xstep == svgtypesetter_note_sep_xstep) {\
            xstep = svgtypesetter_slide_down_xstep;\
        }\
    } else if (*(s) == '\\') {\
        svgtypesetter_flush_slide_up_pinch();\
        if (xstep == NULL || xstep == svgtypesetter_note_sep_xstep) {\
            xstep = svgtypesetter_slide_up_xstep;\
        }\
    } else if (isdigit(*(s))) {\
        svgtypesetter_flush_note_pinch(s);\
    }\
}

    if (txttab->song != NULL) {
        svgtypesetter_spill_song_title(txttab->song);
    }

    if (txttab->transcriber) {
        svgtypesetter_spill_transcriber(txttab->transcriber);
    }

    for (tp = txttab; tp != NULL; tp = tp->next) {
        // TODO(Rafael): Spill sustained techniques and tag times indication.

        for (offset = 0; offset < tp->fretboard_sz; offset++) {

            is_chord = svgtypesetter_is_chord((const char **)tp->strings, offset);

            if (tp->strings[s][offset] == 'b') {
                bend_arrow_string = svgtypesetter_get_bend_arrow_string((const char **)tp->strings, offset);
            } else if (tp->strings[s][offset] == 'r') {
                bend_arrow_string = svgtypesetter_get_release_bend_arrow_string((const char **)tp->strings, offset);
            }

            for (s = 0; s < 6; s++) {
                g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[s].x;
                g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[s].y;
                do_flush_pinch(xstep, &tp->strings[s][offset], s, bend_arrow_string);
            }
        }

        if (xstep == NULL) {
            xstep = svgtypesetter_note_sep_xstep;
        }

        if (xstep != NULL) {
            xstep();
        }

        svgtypesetter_refresh_fbrd_xy();

        if (g_svg_page.tab.fbrd[5].y >= SVGTYPESETTER_PAGE_HEIGHT - (SVGTYPESETTER_TAB_Y_SPAN * 6)) {
            // INFO(Rafael): The current page became full, we need a new one.
            svgtypesetter_newpage();
        }

        if (*g_svg_page.tab.carriage_x >= SVGTYPESETTER_PAGE_WIDTH - SVGTYPESETTER_TAB_X_SPAN) {
            // INFO(Rafael): The current tab diagram became full, we need a new empty one.
            svgtypesetter_spill_tabdiagram();
        }
    }

#undef do_flush_pinch
}

static int svgtypesetter_refresh_fbrd_xy(void) {
    size_t s;
    for (s = 0; s < 6; s++) {
        if (&g_svg_page.tab.fbrd[s].x != g_svg_page.tab.carriage_x) {
            g_svg_page.tab.fbrd[s].x = *g_svg_page.tab.carriage_x;
        }
    }

    for (s = 1; s < 6; s++) {
        g_svg_page.tab.fbrd[s].y = g_svg_page.tab.fbrd[s - 1].y + SVGTYPESETTER_TAB_Y_SPAN;
    }
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

    g_svg_page.tab.fbrd[0].x = g_svg_page.tab.xlim_left;
    g_svg_page.tab.fbrd[0].y = SVGTYPESETTER_TAB_Y;

    g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[0].x;
    g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[0].y;

    svgtypesetter_refresh_fbrd_xy();

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
    y = *g_svg_page.tab.carriage_y;

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
                                                                                        *g_svg_page.tab.carriage_y,
                                                                                        *g_svg_page.tab.carriage_y +
                                                                                            SVGTYPESETTER_TAB_Y_SPAN * 10);

    svgtypesetter_refresh_fbrd_xy();
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

    g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[0].x;
    g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[0].y;

    return 1;
}

int svgtypesetters_svginkspill(const char *filepath, const txttypesetter_tablature_ctx *tab,
                               const tulip_single_note_ctx *song) {
    return 0;
}

