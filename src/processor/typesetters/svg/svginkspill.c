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

static inline void svgtypesetter_hammer_on_xstep(const int delta);

static inline void svgtypesetter_vibrato_xstep(const int delta);

static inline void svgtypesetter_pull_off_xstep(const int delta);

static inline void svgtypesetter_release_bend_xstep(const int delta);

static inline void svgtypesetter_bend_xstep(const int delta);

static inline void svgtypesetter_note_sep_xstep(const int delta);

static inline void svgtypesetter_slide_down_xstep(const int delta);

static inline void svgtypesetter_slide_up_xstep(const int delta);

static inline void svgtypesetter_sep_bar_xstep(const int delta);

static inline void svgtypesetter_chord_span_xstep(const int delta);

static void svgtypesetter_insert_header_span(void);

static void svgtypesetter_spill_song_title(const char *title);

static void svgtypesetter_spill_transcriber(const char *name);

static void svgtypesetter_spill_comments(const txttypesetter_tablature_ctx *txttab);

static void svgtypesetter_flush_note_pinch(const char *note);

static void svgtypesetter_flush_bend_pinch(const int arrow);

static void svgtypesetter_flush_release_bend_pinch(const int arrow);

static void svgtypesetter_flush_vibrato_pinch(void);

static void svgtypesetter_flush_hammer_on_pull_off_pinch(void);

static void svgtypesetter_flush_slide_down_pinch(void);

static void svgtypesetter_flush_slide_up_pinch(void);

static void svgtypesetter_flush_sep_bar(void);

static int svgtypesetter_is_chord(const char **strings, const size_t offset);

static size_t svgtypesetter_get_release_bend_arrow_string(const char **strings, const size_t offset);

static size_t svgtypesetter_get_bend_arrow_string(const char **strings, const size_t offset);

static void do_xstep(int *x, const char **strings, const size_t offset);

static void svgtypesetter_spill_sustained_techniques(const txttypesetter_tablature_ctx *txttab);

static void svgtypesetter_flush_fretboard_pinches(const txttypesetter_tablature_ctx *txttab);

static int svgtypesetter_refresh_fbrd_xy(void);

static int svgtypesetter_newpage(void);

static void svgtypesetter_spill_tabdiagram(void);

static void svgtypesetter_fclose(void);

static int svgtypesetter_init(const char *filename);

static inline void svgtypesetter_newtabdiagram(void);

static struct svgtypesetter_page_ctx g_svg_page;

static inline void svgtypesetter_newtabdiagram(void) {
    g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[0].x;
    g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[0].y;
    *g_svg_page.tab.carriage_x = g_svg_page.tab.xlim_left;
    *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN * 10;
    svgtypesetter_refresh_fbrd_xy();
    svgtypesetter_spill_tabdiagram();
}

static inline void svgtypesetter_hammer_on_xstep(const int delta) {
    *g_svg_page.tab.carriage_x += (SVGTYPESETTER_TAB_X_SPAN + 10) * delta;
}

static inline void svgtypesetter_vibrato_xstep(const int delta) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_TAB_X_SPAN * delta;
}

static inline void svgtypesetter_pull_off_xstep(const int delta) {
    svgtypesetter_hammer_on_xstep(delta);
}

static inline void svgtypesetter_release_bend_xstep(const int delta) {
    svgtypesetter_bend_xstep(delta);
}

static inline void svgtypesetter_bend_xstep(const int delta) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_TAB_X_SPAN * delta;
}

static inline void svgtypesetter_note_sep_xstep(const int delta) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_TAB_X_SPAN * delta;
}

static inline void svgtypesetter_sep_bar_xstep(const int delta) {
    *g_svg_page.tab.carriage_x += (SVGTYPESETTER_TAB_X_SPAN - 5) * delta;
}

static inline void svgtypesetter_slide_down_xstep(const int delta) {
    *g_svg_page.tab.carriage_x += (SVGTYPESETTER_TAB_X_SPAN - 5) * delta;
}

static inline void svgtypesetter_slide_up_xstep(const int delta) {
    svgtypesetter_slide_down_xstep(delta);
}

static inline void svgtypesetter_chord_span_xstep(const int delta) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_CHORD_SPAN_X_STEP * delta;
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
                           " font-style=\"italic\" font-weight=\"bold\">transcribed by %s</text>\n",
                                                                                      *g_svg_page.tab.carriage_x,
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
                                                                                               *g_svg_page.tab.carriage_y +
                                                                                               SVGTYPESETTER_TAB_NOTE_Y_OFFSET);
    while (isdigit(*np)) {
        fprintf(g_svg_page.fp, "%c", *np);
        np++;
    }

    fprintf(g_svg_page.fp, "</text>\n");
}

static void svgtypesetter_flush_bend_pinch(const int arrow) {
    char *arrow_markup = (arrow) ? " marker-end=\"url(#arrow)\"" : "";
    fprintf(g_svg_page.fp, "\t<path d=\"M%d,%d C%d,%d, %d,%d %d,%d\""
                           " fill=\"none\" stroke=\"black\" stroke-width=\"1\"%s/>\n", *g_svg_page.tab.carriage_x,
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
    char *arrow_markup = (arrow) ? " marker-end=\"url(#arrow)\"" : "";
    fprintf(g_svg_page.fp, "\t<path d=\"M%d,%d C%d,%d %d,%d %d,%d\""
                           " fill=\"none\" stroke=\"black\" stroke-width=\"1\"%s/>\n", *g_svg_page.tab.carriage_x,
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
                           " transform=\"rotate(90,%d,%d)\">(</text>\n", *g_svg_page.tab.carriage_x,
                                                                         *g_svg_page.tab.carriage_y,
                                                                         *g_svg_page.tab.carriage_x + 4,
                                                                         *g_svg_page.tab.carriage_y);
}

static void svgtypesetter_flush_slide_down_pinch(void) {
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"18\">/</text>\n", *g_svg_page.tab.carriage_x,
                                                                                    *g_svg_page.tab.carriage_y +
                                                                                SVGTYPESETTER_TAB_NOTE_Y_OFFSET + 2);
}

static void svgtypesetter_flush_slide_up_pinch(void) {
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"18\">\\</text>\n", *g_svg_page.tab.carriage_x,
                                                                                    *g_svg_page.tab.carriage_y +
                                                                                SVGTYPESETTER_TAB_NOTE_Y_OFFSET + 2);
}

static void svgtypesetter_flush_sep_bar(void) {
    int y = g_svg_page.tab.fbrd[0].y;
    fprintf(g_svg_page.fp, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\""
                           " style=\"stroke:rgb(0,0,0);stroke-width:1;opacity:0.5\"/>\n", *g_svg_page.tab.carriage_x,
                                                                                          *g_svg_page.tab.carriage_x,
                                                                                          y,
                                                                                          y + SVGTYPESETTER_TAB_Y_SPAN * 5);
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
        if (y >= SVGTYPESETTER_PAGE_HEIGHT - (SVGTYPESETTER_TAB_Y_SPAN * 6 * n)) {
            // INFO(Rafael): The current page would became full, thus we need a new one, where we can spill sustained
            //               techniques together with the whole tab diagram.
            svgtypesetter_newpage();
        }
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

static void svgtypesetter_flush_fretboard_pinches(const txttypesetter_tablature_ctx *txttab) {
    const txttypesetter_tablature_ctx *tp;
    size_t s, offset;
    void (*xstep)(const int) = NULL;
    void (*last_xstep)(const int) = NULL;
    int bend_arrow_string, spill_done, is_chord;
    char *times, *times_end, tm_buf[20];

#define do_xpack(xspan) {\
    if (last_xstep != NULL) {\
        last_xstep(-1);\
        last_xstep = NULL;\
        *g_svg_page.tab.carriage_x += xspan;\
        svgtypesetter_refresh_fbrd_xy();\
    }\
}

#define do_flush_pinch(xstep, s, sn, arrow_string) {\
    if (*(s) == '~') {\
        do_xpack(7);\
        svgtypesetter_flush_vibrato_pinch();\
        if (xstep == NULL || xstep == svgtypesetter_note_sep_xstep) {\
            xstep = svgtypesetter_vibrato_xstep;\
        }\
    } else if (*(s) == 'b') {\
        do_xpack(7);\
        svgtypesetter_flush_bend_pinch(sn == arrow_string);\
        if (xstep == NULL || xstep == svgtypesetter_note_sep_xstep) {\
            xstep = svgtypesetter_bend_xstep;\
        }\
    } else if (*(s) == 'r') {\
        do_xpack(7);\
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
        if (last_xstep == svgtypesetter_chord_span_xstep) {\
            last_xstep(-1);\
            do_xpack(13);\
        } else {\
            do_xpack(10);\
        }\
        svgtypesetter_flush_slide_down_pinch();\
        if (xstep == NULL || xstep == svgtypesetter_note_sep_xstep) {\
            xstep = svgtypesetter_slide_down_xstep;\
        }\
    } else if (*(s) == '\\') {\
        if (last_xstep == svgtypesetter_chord_span_xstep) {\
            last_xstep(-1);\
            do_xpack(14);\
        } else {\
            do_xpack(11);\
        }\
        svgtypesetter_flush_slide_up_pinch();\
        if (xstep == NULL || xstep == svgtypesetter_note_sep_xstep) {\
            xstep = svgtypesetter_slide_up_xstep;\
        }\
    } else if (*(s) == '|' && sn == 5) {\
        svgtypesetter_flush_sep_bar();\
        xstep = svgtypesetter_sep_bar_xstep;\
    } else if (isdigit(*(s))) {\
        svgtypesetter_flush_note_pinch(s);\
        xstep = svgtypesetter_note_sep_xstep;\
    }\
}

    svgtypesetter_spill_tabdiagram();

    for (tp = txttab; tp != NULL; tp = tp->next) {
        times = tp->times;
        times_end = times + 3;
        for (offset = 0; offset < tp->fretboard_sz; offset++) {

            // INFO(Rafael): Spilling times.
            if (times != NULL && times > times_end && isdigit(*times)) {
                for (times_end = times; *times_end != 'X'; times_end++)
                    ;
                memset(tm_buf, 0, sizeof(tm_buf));
                memcpy(tm_buf, times, times_end - times + 1);
                fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                       " font-size=\"13\" font-weight=\"bold\">%s</text>\n", *g_svg_page.tab.carriage_x,
                                                                                             g_svg_page.tab.fbrd[0].y -
                                                                                             SVGTYPESETTER_TAB_Y_SPAN, tm_buf);
            }

            is_chord = svgtypesetter_is_chord((const char **)tp->strings, offset);

            bend_arrow_string = 6;
            for (s = 0; s < 6; s++) {
                if (tp->strings[s][offset] == '-' ||
                    (offset > 0 && isdigit(tp->strings[s][offset-1]) && isdigit(tp->strings[s][offset]))) {
                    if (is_chord && tp->strings[s][offset] == '-' &&
                        (offset + 1) < tp->fretboard_sz           &&
                        isdigit(tp->strings[s][offset + 1])       &&
                        xstep == svgtypesetter_note_sep_xstep) {
                        xstep = svgtypesetter_chord_span_xstep;
                    }
                    continue;
                }
                if (tp->strings[s][offset] == 'b' && bend_arrow_string == 6) {
                    bend_arrow_string = svgtypesetter_get_bend_arrow_string((const char **)tp->strings, offset);
                } else if (tp->strings[s][offset] == 'r' && bend_arrow_string == 6) {
                    bend_arrow_string = svgtypesetter_get_release_bend_arrow_string((const char **)tp->strings, offset);
                }
                g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[s].x;
                g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[s].y;
                do_flush_pinch(xstep, &tp->strings[s][offset], s, bend_arrow_string);
            }

            if (is_chord && offset > 0) {
                for (s = 0; s < 6 && is_chord; s++) {
                    if (tp->strings[s][offset-1] == '-' && tp->strings[s][offset] == '-') {
                        continue;
                    }
                    is_chord = isdigit(tp->strings[s][offset-1]) && isdigit(tp->strings[s][offset]);
                }
                if (is_chord) {
                    svgtypesetter_chord_span_xstep(1);
                    last_xstep = svgtypesetter_chord_span_xstep;
                    xstep = NULL;
                }
            }

            if (xstep != NULL) {
                xstep(1);
                last_xstep = xstep;
                xstep = NULL;
                svgtypesetter_refresh_fbrd_xy();

                if (g_svg_page.tab.fbrd[5].y >= SVGTYPESETTER_PAGE_HEIGHT - (SVGTYPESETTER_TAB_Y_SPAN * 6) &&
                    tp->next != NULL) {
                    // INFO(Rafael): The current page became full, we need a new one.
                    svgtypesetter_newpage();
                }

                if (*g_svg_page.tab.carriage_x >= SVGTYPESETTER_PAGE_WIDTH - SVGTYPESETTER_TAB_X_SPAN && tp->next != NULL) {
                    // INFO(Rafael): The current tab diagram became full, we need a new empty one.
                    svgtypesetter_newtabdiagram();
                }
            }

            if (times != NULL) {
                times++;
            }
        }
        if (tp->next != NULL) {
            svgtypesetter_newtabdiagram();
        }
    }

#undef do_flush_pinch

#undef do_xpack
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
    snprintf(g_svg_page.curr_pagefile, sizeof(g_svg_page.curr_pagefile) - 1, "%s-%03d-pp.svg", g_svg_page.filename,
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

    fprintf(g_svg_page.fp, "<svg xmlns=\"http://www.w3.org/2000/svg\""
                           " width=\"%d\" height=\"%d\" style=\"background-color:white\">\n"
                           "\t<defs>\n"
                            "\t\t<marker id=\"arrow\" markerWidth=\"10\" markerHeight=\"10\" refX=\"1\" refY=\"4\""
                            " orient=\"auto\" markerUnits=\"strokeWidth\">\n"
                            "\t\t\t<path d=\"M0,0 L0,8 L9,3 z\" fill=\"black\"/>\n"
                            "\t\t</marker>\n"
                            "\t</defs>\n", SVGTYPESETTER_PAGE_WIDTH, SVGTYPESETTER_PAGE_HEIGHT);
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
                           " style=\"stroke:rgb(0,0,0);stroke-width:1;opacity:0.5\"/>\n", x, x, y,
                                                                                          y + SVGTYPESETTER_TAB_Y_SPAN * 5);
    // INFO(Rafael): String lines.
    for (s = 0; s < 6; s++) {
        fprintf(g_svg_page.fp, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\""
                               " style=\"stroke:rgb(0,0,0);stroke-width:1;opacity:0.5\"/>\n", x, g_svg_page.tab.xlim_right,
                                                                                              y, y);
        y += SVGTYPESETTER_TAB_Y_SPAN;
    }

    // INFO(Rafael): Vertical right line.
    fprintf(g_svg_page.fp, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\""
                           " style=\"stroke:rgb(0,0,0);stroke-width:1;opacity:0.5\"/>\n", g_svg_page.tab.xlim_right,
                                                                                          g_svg_page.tab.xlim_right,
                                                                                          *g_svg_page.tab.carriage_y,
                                                                                          *g_svg_page.tab.carriage_y +
                                                                                            SVGTYPESETTER_TAB_Y_SPAN * 5);
    g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[0].x;
    *g_svg_page.tab.carriage_x = x + SVGTYPESETTER_TAB_X_SPAN;

    svgtypesetter_refresh_fbrd_xy();
}

static void svgtypesetter_fclose(void) {
    char pn[20];
    snprintf(pn, sizeof(pn) - 1, "-%d-", g_svg_page.page_nr);
    if (g_svg_page.fp != NULL) {
        fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" fill=\"black\""
                               " font-size=\"13\" font-style=\"italic\">%s</text>\n", SVGTYPESETTER_PAGE_WIDTH / 2 -
                                                                                        (strlen(pn) - 1),
                                                                                      SVGTYPESETTER_PAGE_HEIGHT -
                                                                                        SVGTYPESETTER_TAB_Y_SPAN + 5,
                                                                                      pn);
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

int svgtypesetter_inkspill(const char *filepath, const txttypesetter_tablature_ctx *tab,
                           const tulip_single_note_ctx *song) {

    if (!svgtypesetter_init(filepath)) {
        return 0;
    }

    if (!svgtypesetter_newpage()) {
        return 0;
    }

    if (tab->song != NULL) {
        svgtypesetter_spill_song_title(tab->song);
    }

    if (tab->transcriber) {
        svgtypesetter_spill_transcriber(tab->transcriber);
    }

    svgtypesetter_flush_fretboard_pinches(tab);

    svgtypesetter_fclose();

    return 1;
}