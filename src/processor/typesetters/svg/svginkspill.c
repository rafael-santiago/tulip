/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/svg/svginkspill.h>
#include <processor/typesetters/svg/svgboundaries.h>
#include <processor/typesetters/svg/svg_do_flush_pinch.h>
#include <processor/typesetters/typeprefs.h>
#include <processor/oututils.h>
#include <processor/settings.h>
#include <base/types.h>
#include <base/memory.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

// TIP(Rafael): 'Pinch', 'pinched' here is a lousy pun for 'punched' from punched cards ;)
//              When you read TAB, please do not scream, I am just using the standard way of write it here.

// INFO(Rafael): This structure represents the current (x;y) coordinate of a string represented inside a TAB diagram.
struct svgtypesetter_pinch_point_ctx {
    int x, y;
};

// INFO(Rafael): This structure gathers some relevant info used to perform carriage returns when necessary.
struct last_note_info_ctx {
    int x;
    int len;
    int do_cr_arg;
    void (*do_carriage_return)(const int, int *);
};

// INFO(Rafael): This structure represents a TAB diagram, e.g:
//
// |--------------...[0]
// |-T------------...[1]
// |--------------...[2]
// |-A------------...[3]
// |--------------...[4]
// |-B------------...[5]
//
struct svgtypesetter_tab_diagram_ctx {
    // INFO(Rafael): Tab margin left and right.
    int xlim_left, xlim_right;
    // INFO(Rafael): Points to the (x;y) of the current processed string.
    int *carriage_x, *carriage_y, *last_carriage_x, *last_carriage_y;
    // INFO(Rafael): All twelve (x;y) points of the fretboard.
    struct svgtypesetter_pinch_point_ctx fbrd[6];
    // INFO(Rafael): Stores the last x coordinate pinched with a non null sep ('-').
    int last_non_null_x;
    // INFO(Rafael): This will store infos from each processed strings.
    struct last_note_info_ctx ln_info[6], *curr_ln_info;
    // INFO(Rafael): Just a buffer that records the last processed relevant symbol.
    tulip_command_t last_symbol;
};

struct svgtypesetter_page_ctx {
    // INFO(Rafael): Points to the current SVG file (in this case a page from 001 to NNN). If your tab has more than 999 pages
    //               I am really sorry but it is not for you.
    FILE *fp;
    // INFO(Rafael): Points to the current section of the ascii TAB
    const txttypesetter_tablature_ctx *tp;
    int y; // DEPRECATED(Rafael): wtf...
    // INFO(Rafael): Some counting stuff for give us some clues to make some assumptions.
    int page_nr, tab_per_page_nr;
    // INFO(Rafael): filename stores the base name 'Foobar.svg' the curr_pagefile stores the effective numbered file name
    // 'Foobar-001.svg'.
    char filename[4096], curr_pagefile[4096];
    // INFO(Rafael): It represents the TAB diagram drawed or being drawn into current written SVG file.
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

static inline void svgtypesetter_user_note_span_xstep(const int delta);

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

static void svgtypesetter_flush_fretboard_pinches(txttypesetter_tablature_ctx *txttab);

static int svgtypesetter_refresh_fbrd_xy(void);

static int svgtypesetter_newpage(void);

static void svgtypesetter_cut_tab(void);

static void svgtypesetter_spill_tabdiagram(void);

static void svgtypesetter_fclose(void);

static int svgtypesetter_init(const char *filename);

static void svgtypesetter_newtabdiagram(const txttypesetter_tablature_ctx *txttab);

static int has_unflushed_data(const char **strings, const size_t offset, const size_t fretboard_size);

static void svgtypesetter_reduce_blank_yspan(const txttypesetter_tablature_ctx *txttab);

static void svgtypesetter_spill_tab_notation(const tulip_single_note_ctx *song);

static void svgtypesetter_spill_tuning(void);

static void svgtypesetter_clean_old_pages_not_rewritten(void);

static void svgtypesetter_normalize_ascii_tab(txttypesetter_tablature_ctx *txttab);

static void svgtypesetter_move_carriage_to_best_fit(const int space_amount);

static void svgtypesetter_carriage_return(const int space_amount, int *temp);

static struct svgtypesetter_page_ctx g_svg_page; // INFO(Rafael): The current written page with all control info.

int svgtypesetter_inkspill(const char *filepath, txttypesetter_tablature_ctx *tab,
                           const tulip_single_note_ctx *song) {

    // INFO(Rafael): Here you will find the general processing flow followed by this module.
    //
    //               The general idea is:
    //
    //                     i. Initialise the SVG output stream by creating the first page file descriptor,
    //                        setting default values for g_svg_page.
    //                    ii. Ask for a new page.
    //                   iii. Write song name into the SVG if a song name was given.
    //                    iv. Write transcriber's name into the SVG if it was given.
    //                     v. Write the TAB notation gathering all used techniques if it was asked by the user.
    //                    vi. Write the used tuning if it was asked by the user.
    //                   vii. Insert a header span if song or transcriber data was written.
    //                  viii. Pinch all notes into the SVG tab diagram following the passed ascii TAB (Err... more on later).
    //                    ix. Close the last generated SVG page file.
    //                     x. If exist old pages from a previous processing, erase all them.
    //
    //                                                          'Tsc! roman numbers are so...romans.'
    //                                                                             -- An ancient greek philosopher.

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

    svgtypesetter_spill_tab_notation(song);

    svgtypesetter_spill_tuning();

    if (tab->song != NULL || tab->transcriber != NULL) {
        svgtypesetter_insert_header_span();
    }

    svgtypesetter_flush_fretboard_pinches(tab);

    svgtypesetter_fclose();

    svgtypesetter_clean_old_pages_not_rewritten();

    return 1;
}

static void svgtypesetter_newtabdiagram(const txttypesetter_tablature_ctx *txttab) {
    int comments_nr = 0, s_techniques_nr = 0, has_times = 0, tab_auto_break = 0, y, y0, y1;
    const txttypesetter_sustained_technique_ctx *sp;
    const txttypesetter_comment_ctx *cp;
    char *p, *p_end;

    // INFO(Rafael): This function draws a new TAB diagram taking into consideration the current
    //               y coordinate of the current page, however, it does not directly use the coordinate.
    //               The function performs some calculation based on what should be flushed and
    //               sometimes what was previously flushed.

    y0 = g_svg_page.tab.fbrd[0].y;
    y1 = g_svg_page.tab.fbrd[5].y;

    // INFO(Rafael): A 'TAB auto break' occurs when the ascii tab diagram not fits into the SVG diagram.
    //               Bearmind that ascii TAB assumes monospaced fonts and all technique symbols are represented
    //               by only one ascii symbol, while a SVG TAB will use non-ascii symbols for some techniques.

    tab_auto_break = (g_svg_page.tab.last_non_null_x >= (SVGTYPESETTER_PAGE_WIDTH - SVGTYPESETTER_TAB_X_SPAN));

    // INFO(Rafael): If it was an auto break could have more data in the same coordinate waiting for being 'pinched'.

    g_svg_page.tab.last_carriage_x = g_svg_page.tab.carriage_x;
    g_svg_page.tab.last_carriage_y = g_svg_page.tab.carriage_y;

    // INFO(Rafael): Since it is a new TAB diagram we will start write it from left to the right.
    //               There is no doubt, by now it is empty.

    g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[0].x;
    *g_svg_page.tab.carriage_x = g_svg_page.tab.xlim_left;

    // INFO(Rafael): This if-else block is for selecting the best y coordinate for the new TAB diagram.

    if (txttab->last != NULL && !tab_auto_break &&
        !has_unflushed_data((const char **)txttab->last->strings, 0, txttab->last->fretboard_sz)) {
        // INFO(Rafael): This if-clause covers the case where we got, as instance, '.literal{"blah blah"};'
        //               that will generate an empty TAB diagram. In this case, we can assume the last fbrd[0].y
        //               coordinate to get the next. But if it is an auto break, it indicates that there is TAB
        //               data drawn above and in this case, fbrd[5].y must be considered instead.
        //*g_svg_page.tab.carriage_x = g_svg_page.tab.xlim_left; DEPRECATED(Rafael): wtf...
        g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[0].y;
    } else {
        if (txttab->last == NULL && !tab_auto_break) {
            // INFO(Rafael): The very first TAB chunk.
            g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[0].y;
        } else {
            // INFO(Rafael): An auto break of the first TAB chunk, an auto break or a subsequent TAB chunk.
            g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[5].y;
        }
    }

    // INFO(Rafael): Verifying if this new TAB diagram has sustained techniques. They will drawn above
    //               TAB diagram by using some heuristic fill spacing.

    for (sp = txttab->techniques; sp != NULL; sp = sp->next) {
        s_techniques_nr++;
    }

    if (!tab_auto_break) {
        // INFO(Rafael): If it is not an auto break, we must also consider the comments. Because they are flushed
        //               before the TAB diagram and sustained techniques indication.
        for (cp = txttab->comments; cp != NULL; cp = cp->next) {
            p = cp->data;
            p_end = p + strlen(p);
            while (p != p_end) {
                // INFO(Rafael): We must consider returns because they virtually become new '.literal{""}' statements here
                //               in this processor.
                comments_nr += (*p == '\n');
                p++;
            }
            comments_nr++;
        }
    }

    // INFO(Rafael): Verifying if this new TAB diagram has some time indication. It will put be immediately above the TAB
    //               diagram.

    p = txttab->times;
    p_end = p + strlen(p);
    while (!has_times && p != p_end) {
        has_times = isdigit(*p);
        p++;
    }

    // INFO(Rafael): The y equation is a bit heuristic but becomes evident if you consider:
    //
    //                  [COMMENT AREA]
    //                  [SUSTAINED TECHNIQUES AREA]
    //                  [TIMES AREA]
    //                  [TAB DIAGRAM]
    //
    //  [] -> Flushed when it exists.

    *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN +
                                  ((tab_auto_break) ? SVGTYPESETTER_TAB_Y_SPAN : 0) +
                                  ((s_techniques_nr > 0) ? SVGTYPESETTER_TAB_Y_SPAN * 2 : 0) +
                                  ((SVGTYPESETTER_TAB_Y_SPAN * 2) * s_techniques_nr) +
                                  ((SVGTYPESETTER_TAB_Y_SPAN + 2) * comments_nr + 10) +
                                  ((has_times) ? SVGTYPESETTER_TAB_Y_SPAN * 2 : 0);

    // INFO(Rafael): The refresh function considers the fbrd[0].y coordinate to do the relative update
    //               over remaining strings coordinates.

    g_svg_page.tab.fbrd[0].y = *g_svg_page.tab.carriage_y;
    svgtypesetter_refresh_fbrd_xy();

    if (g_svg_page.tab.fbrd[5].y >= (SVGTYPESETTER_PAGE_HEIGHT - (SVGTYPESETTER_TAB_Y_SPAN * 6))) {
        // INFO(Rafael): The current page became full, we need a new one.
        svgtypesetter_newpage();
    } else if (tab_auto_break) {
        // INFO(Rafael): When a auto break occurs, sometimes it can result in an overlap. Let's try to
        //               mitigate it.
        if (s_techniques_nr > 0) {
            y = g_svg_page.tab.fbrd[0].y;
            y -= (SVGTYPESETTER_TAB_Y_SPAN * 2) - 10;
        }

        if (y >= y0 && y <= y1) {
            // INFO(Rafael): The heuristic space was not so good we still have an overlapping.
            y = y1 + SVGTYPESETTER_TAB_Y_SPAN + ((s_techniques_nr == 0) ? 5 : 0);
        }

        g_svg_page.tab.fbrd[0].y = y;
        svgtypesetter_refresh_fbrd_xy();

        if (g_svg_page.tab.fbrd[5].y >= (SVGTYPESETTER_PAGE_HEIGHT - (SVGTYPESETTER_TAB_Y_SPAN * 6))) {
            // INFO(Rafael): The current page became full, we need a new one.
            svgtypesetter_newpage();
        }
    }

//svgtypesetter_newtabdiagram_epilogue: // DEPRECATED(Rafael): wtf...

    // INFO(Rafael): Finally drawing the TAB diagram.
    svgtypesetter_spill_tabdiagram();

    // INFO(Rafael): Restore the coordinate pointers state as before asking for a new TAB diagram.
    g_svg_page.tab.carriage_x = g_svg_page.tab.last_carriage_x;
    g_svg_page.tab.carriage_y = g_svg_page.tab.last_carriage_y;
}

static void svgtypesetter_spill_comments(const txttypesetter_tablature_ctx *txttab) {
    const txttypesetter_comment_ctx *cp;
    char comment[65535];
    char *p, *p_end, *lp;
    int y;

    if (txttab->last == NULL || g_svg_page.tab_per_page_nr == 0) {
     y = g_svg_page.tab.fbrd[0].y;
    } else {
        if (has_unflushed_data((const char **)txttab->last->strings, 0, txttab->last->fretboard_sz)) {
            y = g_svg_page.tab.fbrd[5].y;
            y += (SVGTYPESETTER_TAB_Y_SPAN + 2) * 2;
        } else {
            y = g_svg_page.tab.fbrd[0].y;
            y += SVGTYPESETTER_TAB_Y_SPAN + 2;
        }
        if (y >= (SVGTYPESETTER_PAGE_HEIGHT - (SVGTYPESETTER_TAB_Y_SPAN * 6))) {
            // INFO(Rafael): The current page became full, we need a new one.
            svgtypesetter_newpage();
            y = g_svg_page.tab.fbrd[0].y;
        }
    }

    cp = txttab->comments;
    p = lp = cp->data;
    p_end = p + strlen(p);

    while (cp != NULL) {
        while (lp != p_end && *lp != '\n') {
            lp++;
        }
        memset(comment, 0, sizeof(comment));
        if (p != NULL) {
            memcpy(comment, p, lp - p);
        }
        p = lp + 1;
        if (strlen(comment) > 0) {
            fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" fill=\"black\" font-size=\"11\">"
                                   "%s</text>\n", g_svg_page.tab.xlim_left,
                                                  y,
                                                  comment);
        }
        y += SVGTYPESETTER_TAB_Y_SPAN + 2;
        if (y >= (SVGTYPESETTER_PAGE_HEIGHT - (SVGTYPESETTER_TAB_Y_SPAN * 6))) {
            // INFO(Rafael): The current page became full, we need a new one.
            svgtypesetter_newpage();
            y = g_svg_page.tab.fbrd[0].y;
        }
        if (p >= p_end) {
            cp = cp->next;
            if (cp != NULL) {
                p = lp = cp->data;
                p_end = p + strlen(p);
            }
        } else {
            lp = p;
        }
    }

    if (!has_unflushed_data((const char **)txttab->strings, 0, txttab->fretboard_sz)) {
        g_svg_page.tab.fbrd[0].y = y;
        svgtypesetter_refresh_fbrd_xy();
    }
}

static void svgtypesetter_flush_fretboard_pinches(txttypesetter_tablature_ctx *txttab) {
    txttypesetter_tablature_ctx *tp;
    size_t s, offset, null_nr = 0;
    void (*xstep)(const int) = NULL;
    void (*last_xstep)(const int) = NULL;
    struct notes_span_ctx {
        int is_beyond_9th_fret[6];
        int do_span;
    } notes_span;
    int bend_arrow_string, spill_done, is_chord;
    char *times, *times_end, tm_buf[20];
    struct sustained_techniques_points_ctx {
        int x;
        int y;
        char *data;
        char *data_end;
        int print_line;
    } stech_pts[20], *stech_p, *stech_end;
    txttypesetter_sustained_technique_ctx *sp;

    for (tp = txttab; tp != NULL; tp = tp->next) {
        g_svg_page.tp = tp;

        if (tp->comments != NULL) {
            svgtypesetter_spill_comments(tp);
        }

        if (!has_unflushed_data((const char **)tp->strings, 0, tp->fretboard_sz))  {
            continue;
        }

        svgtypesetter_normalize_ascii_tab(tp);

        svgtypesetter_newtabdiagram(tp);

        times = tp->times;
        times_end = times + 3;

        if (tp->techniques != NULL) {
            stech_p = &stech_pts[0];
            stech_end = stech_p + sizeof(stech_pts) / sizeof(stech_pts[0]);
            sp = tp->techniques;
            while (stech_p != stech_end && sp != NULL) {
                stech_p->x = *g_svg_page.tab.carriage_x;
                stech_p->data = sp->data;
                stech_p->data_end = stech_p->data + strlen(stech_p->data);
                stech_p->print_line = 0;
                stech_p++;
                sp = sp->next;
            }
            stech_end = stech_p;
            stech_p = &stech_pts[0];
            stech_p->y = g_svg_page.tab.fbrd[0].y - (SVGTYPESETTER_TAB_Y_SPAN * 2) - 10;
            stech_p++;
            while (stech_p != stech_end) {
                stech_p[0].y = stech_p[-1].y - SVGTYPESETTER_TAB_Y_SPAN;
                stech_p++;
            }
        } else {
            stech_p = stech_end = NULL;
        }

        for (offset = 0; offset < tp->fretboard_sz; offset++) {
            // INFO(Rafael): Spilling sustained techniques.
            if (stech_end != NULL) {
                for (stech_p = &stech_pts[0]; stech_p != stech_end; stech_p++) {
                    if (stech_p->data >= stech_p->data_end) {
                        continue;
                    }
                    if (!stech_p->print_line && isalpha(*stech_p->data)) {
                        stech_p->x = *g_svg_page.tab.carriage_x;
                        fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                               " font-size=\"13\" font-weight=\"bold\">%c%c%c</text>\n", stech_p->x,
                                                                                                         stech_p->y,
                                                                                                         stech_p->data[0],
                                                                                                         stech_p->data[1],
                                                                                                         stech_p->data[2]);
                        stech_p->print_line = 1;
                    } else if (stech_p->print_line && stech_p->data[0] == '.' && stech_p->data[1] != '.') {
                        fprintf(g_svg_page.fp, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\""
                                               " style=\"stroke:rgb(0,0,0);stroke-width:1;opacity:0.5\""
                                               " stroke-dasharray=\"5,5\"/>\n", stech_p->x, *g_svg_page.tab.carriage_x,
                                                                                stech_p->y + 1, stech_p->y + 1);
                        if ((stech_p->data + 2) < stech_p->data_end &&
                            stech_p->data[1] == ' ' && tp->strings[0][offset + 1] == '|' && stech_p->data[2] == '.') {
                            stech_p->x = *g_svg_page.tab.carriage_x;
                        } else {
                            stech_p->print_line = 0;
                        }
                    }
                    stech_p->data++;
                }
            }

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
                *g_svg_page.tab.carriage_x += SVGTYPESETTER_TAB_X_SPAN;
                svgtypesetter_refresh_fbrd_xy();
            }

            is_chord = svgtypesetter_is_chord((const char **)tp->strings, offset);

            memset(&notes_span, 0, sizeof(notes_span));

            if (is_chord) {
                for (s = 0; s < 6; s++) {
                    notes_span.is_beyond_9th_fret[s] = (isdigit(tp->strings[s][offset]) &&
                                                        isdigit(tp->strings[s][offset + 1]));
                    if (!notes_span.do_span && notes_span.is_beyond_9th_fret[s]) {
                        notes_span.do_span = 1;
                    }
                }
            }

            bend_arrow_string = 6;
            //memset(g_svg_page.tab.xnote, 0, sizeof(g_svg_page.tab.xnote));
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
                g_svg_page.tab.curr_ln_info = &g_svg_page.tab.ln_info[s];
                do_flush_pinch(xstep, &tp->strings[s][offset], s, bend_arrow_string);
            }

            if (xstep != NULL) {
                xstep(1);
                last_xstep = xstep;
                xstep = NULL;
                svgtypesetter_refresh_fbrd_xy();

                g_svg_page.tab.last_non_null_x = *g_svg_page.tab.carriage_x;

                if (g_svg_page.tab.fbrd[5].y >= (SVGTYPESETTER_PAGE_HEIGHT - (SVGTYPESETTER_TAB_Y_SPAN * 6)) &&
                    tp->next != NULL) {
                    // INFO(Rafael): The current page became full, we need a new one.
                    svgtypesetter_newpage();
                }

                if (*g_svg_page.tab.carriage_x >= (SVGTYPESETTER_PAGE_WIDTH - SVGTYPESETTER_TAB_X_SPAN) &&
                    has_unflushed_data((const char **)tp->strings, offset + 1, tp->fretboard_sz)) {
                    if (stech_end != NULL) {
                        stech_p = &stech_pts[0];
                        while (stech_p != stech_end) {
                            fprintf(g_svg_page.fp, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\""
                                               " style=\"stroke:rgb(0,0,0);stroke-width:1;opacity:0.5\""
                                               " stroke-dasharray=\"5,5\"/>\n", stech_p->x, *g_svg_page.tab.carriage_x,
                                                                                stech_p->y + 1, stech_p->y + 1);
                            stech_p++;
                        }
                    }
                    // INFO(Rafael): The current tab diagram became full, we need a new empty one.
                    svgtypesetter_newtabdiagram(tp);
                    if (stech_end != NULL) {
                        stech_p = &stech_pts[0];
                        stech_p->x = *g_svg_page.tab.carriage_x;
                        stech_p->y = g_svg_page.tab.fbrd[0].y - (SVGTYPESETTER_TAB_Y_SPAN * 2) - 10;
                        stech_p += 1;
                        while (stech_p != stech_end) {
                            stech_p->x = *g_svg_page.tab.carriage_x;
                            stech_p[0].y = stech_p[-1].y - SVGTYPESETTER_TAB_Y_SPAN;
                            stech_p++;
                        }
                    }
                }
            } else {
                last_xstep = svgtypesetter_user_note_span_xstep;
                svgtypesetter_user_note_span_xstep(1);
                svgtypesetter_refresh_fbrd_xy();
            }

            if (times != NULL) {
                times++;
            }
        }

        g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[0].x;
        *g_svg_page.tab.carriage_x = g_svg_page.tab.last_non_null_x;
        if (last_xstep != NULL) {
            last_xstep(-1);
        }
        svgtypesetter_refresh_fbrd_xy();
        svgtypesetter_cut_tab();
        g_svg_page.tab.last_symbol = kTlpNone;
    }

    g_svg_page.tp = NULL;
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

static inline void svgtypesetter_user_note_span_xstep(const int delta) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_USER_NOTE_SPAN_X_STEP;
}

static void svgtypesetter_insert_header_span(void) {
    *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN * 2;
    svgtypesetter_refresh_fbrd_xy();
}

static void svgtypesetter_spill_song_title(const char *title) {
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" fill=\"black\" font-size=\"30\""
                           " font-weight=\"bold\">%s</text>\n", *g_svg_page.tab.carriage_x,
                                                                *g_svg_page.tab.carriage_y, title);
    *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN * 2;
    svgtypesetter_refresh_fbrd_xy();
}

static void svgtypesetter_spill_transcriber(const char *name) {
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" fill=\"black\" font-size=\"13\""
                           " font-weight=\"bold\">transcribed by %s</text>\n", *g_svg_page.tab.carriage_x,
                                                                               *g_svg_page.tab.carriage_y, name);
    *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN * 2;
    svgtypesetter_refresh_fbrd_xy();
}

static void svgtypesetter_carriage_return(const int space_amount, int *temp) {
    if (temp != NULL) {
        *temp = *g_svg_page.tab.carriage_x;
    }
    *g_svg_page.tab.carriage_x = g_svg_page.tab.curr_ln_info->x + space_amount;
}

static void svgtypesetter_flush_note_pinch(const char *note) {
    const char *np = note;
    int temp = 0;

    if (g_svg_page.tab.curr_ln_info->do_carriage_return != NULL && g_svg_page.tab.curr_ln_info->x > 0) {
        g_svg_page.tab.curr_ln_info->do_carriage_return(g_svg_page.tab.curr_ln_info->do_cr_arg, &temp);
    }

    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"13\" font-weight=\"bold\">", *g_svg_page.tab.carriage_x,
                                                                                               *g_svg_page.tab.carriage_y +
                                                                                               SVGTYPESETTER_TAB_NOTE_Y_OFFSET);
    if (temp > 0) {
        *g_svg_page.tab.carriage_x = temp;
    }

    while ((isdigit(*np) || *np == 'X' || *np == '?') && (np - note) < 2) {
        fprintf(g_svg_page.fp, "%c", *np);
        np++;
    }
    g_svg_page.tab.curr_ln_info->len = (np - note);
    fprintf(g_svg_page.fp, "</text>\n");
}

static void svgtypesetter_flush_bend_pinch(const int arrow) {
    // TODO(Rafael): Verify if find for best carriage fit is necessary here.
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
    // TODO(Rafael): Verify if find for best carriage fit is necessary here.
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

static void svgtypesetter_move_carriage_to_best_fit(const int space_amount) {
    int rl;
    if (g_svg_page.tab.curr_ln_info == NULL || g_svg_page.tab.curr_ln_info->x == 0) {
        return;
    }
    rl = space_amount * g_svg_page.tab.curr_ln_info->len;
    if ((*g_svg_page.tab.carriage_x - g_svg_page.tab.curr_ln_info->x) > rl) {
        do {
            *g_svg_page.tab.carriage_x -= 1;
        } while ((*g_svg_page.tab.carriage_x - g_svg_page.tab.curr_ln_info->x) > rl);
    }
    g_svg_page.tab.curr_ln_info->do_carriage_return = svgtypesetter_carriage_return;
    g_svg_page.tab.curr_ln_info->do_cr_arg = space_amount;
}

static void svgtypesetter_flush_vibrato_pinch(void) {
    svgtypesetter_move_carriage_to_best_fit(7);
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
    svgtypesetter_move_carriage_to_best_fit(7);
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"18\">/</text>\n", *g_svg_page.tab.carriage_x,
                                                                                    *g_svg_page.tab.carriage_y +
                                                                                SVGTYPESETTER_TAB_NOTE_Y_OFFSET + 2);
}

static void svgtypesetter_flush_slide_up_pinch(void) {
    svgtypesetter_move_carriage_to_best_fit(7);
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"18\">\\</text>\n", *g_svg_page.tab.carriage_x,
                                                                                    *g_svg_page.tab.carriage_y +
                                                                                SVGTYPESETTER_TAB_NOTE_Y_OFFSET + 2);
}

static void svgtypesetter_flush_sep_bar(void) {
    int y = g_svg_page.tab.fbrd[0].y;
    // TODO(Rafael): Set to best fit before drawing the bar.
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

static void svgtypesetter_reduce_blank_yspan(const txttypesetter_tablature_ctx *txttab) {
    const txttypesetter_comment_ctx *cp;
    size_t cm_nr = 0;
    char *p, *p_end;

    if (txttab == NULL) {
        return;
    }

    if (txttab->next != NULL && !has_unflushed_data((const char **)txttab->next->strings, 0, txttab->next->fretboard_sz)) {
        return;
    }

    g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[0].y;

    for (cp = txttab->comments; cp != NULL; cp = cp->next) {
        cm_nr++;
    }

    *g_svg_page.tab.carriage_y -= (SVGTYPESETTER_TAB_Y_SPAN * cm_nr) * 6 + SVGTYPESETTER_TAB_Y_SPAN * 2;

    svgtypesetter_refresh_fbrd_xy();
}

static int has_unflushed_data(const char **strings, const size_t offset, const size_t fretboard_size) {
    size_t s, o;
    int has;

    if (offset >= fretboard_size) {
        return 0;
    }

    has = 0;
    for (o = offset; o < fretboard_size && !has; o++) {
        for (s = 0; s < 6 && !has; s++) {
            has = (strings[s][o] != '-');
        }
    }

    return has;
}

static void svgtypesetter_normalize_ascii_tab(txttypesetter_tablature_ctx *txttab) {
    size_t s, offset;
    int has_bad_align;
    char *temp;
    char *sp, *sp_end;

    // INFO(Rafael): This function seeks to avoid bad acceptable alignments in ascii tab. Since this processor directly reads
    //               the ascii output, any wrong alignment can screw up the svg output badly. Here we will search for
    //               consecutive notes without an explicity separator and add one single dash between them:
    //
    //    |--------|
    //    |--------|
    //    |-9-9-9--|
    //    |-101010-|
    //    |--8-8-8-|
    //    |--------|
    //
    //  It will turn into:
    //
    //    |----------|
    //    |----------|
    //    |-9--9--9--|
    //    |-10-10-10-|
    //    |--8--8--8-|
    //    |----------|
    //
    // The SVG processor will beautify it a little more later.
    //

    for (offset = 0; offset < txttab->fretboard_sz - 2; offset++) {
        has_bad_align = 0;
        for (s = 0; s < 6 && !has_bad_align; s++) {
            has_bad_align = (isdigit(txttab->strings[s][offset]) && isdigit(txttab->strings[s][offset + 1]) &&
                             isdigit(txttab->strings[s][offset + 2]));
        }
        if (has_bad_align) {
            for (s = 0; s < 6; s++) {
                temp = (char *) getseg(txttab->fretboard_sz + 4);
                memset(temp, 0, txttab->fretboard_sz + 4);
                memcpy(temp, txttab->strings[s], offset + 2);
                temp[offset + 2] = '-';
                memcpy(&temp[offset + 3], &txttab->strings[s][offset + 2], txttab->fretboard_sz - offset);
                free(txttab->strings[s]);
                txttab->strings[s] = temp;
            }
            offset -= 1;
            txttab->fretboard_sz += 1;
        }
    }
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

static void svgtypesetter_cut_tab(void) {
    int x;
    size_t s;
    if (g_svg_page.fp == NULL) {
        return;
    }
    if (g_svg_page.tab.last_symbol != kTlpSepBar) {
        svgtypesetter_flush_sep_bar();
    } else {
        *g_svg_page.tab.carriage_x = g_svg_page.tab.ln_info[0].x;
    }
    fprintf(g_svg_page.fp, "\t<rect x=\"%d\" y=\"%d\" width=\"%d\""
                           " height=\"%d\" fill=\"white\"/>\n", *g_svg_page.tab.carriage_x + 1,
                                                                g_svg_page.tab.fbrd[0].y - SVGTYPESETTER_TAB_Y_SPAN,
                                                                SVGTYPESETTER_PAGE_WIDTH - *g_svg_page.tab.carriage_x + 1,
                                                                SVGTYPESETTER_TAB_Y_SPAN * 6 + SVGTYPESETTER_TAB_Y_SPAN / 2);
}

static int svgtypesetter_newpage(void) {
    size_t s;
    if (g_svg_page.tp != NULL && g_svg_page.tp->last != NULL &&
        has_unflushed_data((const char **)g_svg_page.tp->last->strings, 0, g_svg_page.tp->last->fretboard_sz)) {
        svgtypesetter_cut_tab();
    }

    if (g_svg_page.fp != NULL) {
        svgtypesetter_fclose();
    }

    snprintf(g_svg_page.curr_pagefile, sizeof(g_svg_page.curr_pagefile) - 1, "%s-%03d.svg", g_svg_page.filename,
                                                                                            g_svg_page.page_nr);

    if ((g_svg_page.fp = fopen(g_svg_page.curr_pagefile, "w")) == NULL) {
        return 0;
    }

    g_svg_page.tab.xlim_left = SVGTYPESETTER_TAB_XL_DELTA;
    g_svg_page.tab.xlim_right = SVGTYPESETTER_PAGE_WIDTH + SVGTYPESETTER_TAB_XR_DELTA;

    g_svg_page.tab.fbrd[0].x = g_svg_page.tab.xlim_left;
    g_svg_page.tab.fbrd[0].y = SVGTYPESETTER_TAB_Y;

    g_svg_page.tab.last_carriage_x = g_svg_page.tab.carriage_x;
    g_svg_page.tab.last_carriage_y = g_svg_page.tab.carriage_y;

    g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[0].x;
    g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[0].y;

    for (s = 0; s < 6; s++) {
        g_svg_page.tab.ln_info[s].x = *g_svg_page.tab.carriage_x;
        g_svg_page.tab.ln_info[s].len = 0;
    }

    svgtypesetter_refresh_fbrd_xy();

    // TODO(Rafael): Read encoding scheme from user option letting (ISO-8859-1) as the default option.
    fprintf(g_svg_page.fp, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>\n"
                           "<svg xmlns=\"http://www.w3.org/2000/svg\""
                           " width=\"%d\" height=\"%d\" style=\"background-color:white\">\n"
                           "\t<rect x=\"1\" y=\"1\" width=\"%d\" height=\"%d\" fill=\"white\"/>\n"
                           "\t<defs>\n"
                            "\t\t<marker id=\"arrow\" markerWidth=\"10\" markerHeight=\"10\" refX=\"1\" refY=\"4\""
                            " orient=\"auto\" markerUnits=\"strokeWidth\">\n"
                            "\t\t\t<path d=\"M0,0 L0,8 L9,3 z\" fill=\"black\"/>\n"
                            "\t\t</marker>\n"
                            "\t</defs>\n", SVGTYPESETTER_PAGE_WIDTH, SVGTYPESETTER_PAGE_HEIGHT,
                                           SVGTYPESETTER_PAGE_WIDTH, SVGTYPESETTER_PAGE_HEIGHT);

    g_svg_page.tab_per_page_nr = 1;

    g_svg_page.tab.carriage_x = g_svg_page.tab.last_carriage_x;
    g_svg_page.tab.carriage_y = g_svg_page.tab.last_carriage_y;

    return 1;
}

static void svgtypesetter_spill_tabdiagram(void) {
    int x, y, s, has_semi = 0;
    struct typesetter_curr_settings cset = typesetter_settings();

    g_svg_page.tab_per_page_nr++;

    y = g_svg_page.tab.fbrd[0].y;

    if (cset.prefs & kTlpPrefsAddTuningToTheFretboard) {
        g_svg_page.tab.xlim_left = x = SVGTYPESETTER_TAB_XL_DELTA;
        for (s = 0; s < g_svg_page.tp->string_nr; s++) {
            fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" fill=\"black\""
                                   " font-size=\"10\">%s</text>\n", x, y, g_svg_page.tp->tuning[s]);
            if (!has_semi) {
                has_semi = strlen(g_svg_page.tp->tuning[s]) > 1;
            }
            y += SVGTYPESETTER_TAB_Y_SPAN + 1;
        }
        g_svg_page.tab.xlim_left += SVGTYPESETTER_TAB_X_SPAN - 5 + ((has_semi) ? 5 : 0);
        x = g_svg_page.tab.xlim_left;
        y = g_svg_page.tab.fbrd[0].y;
    } else {
        x = g_svg_page.tab.xlim_left;
    }

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
                                                                                          g_svg_page.tab.fbrd[0].y,
                                                                                          g_svg_page.tab.fbrd[0].y + SVGTYPESETTER_TAB_Y_SPAN * 5);
    g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[0].x;
    *g_svg_page.tab.carriage_x = x + SVGTYPESETTER_TAB_X_SPAN;

    for (s = 0; s < 6; s++) {
        g_svg_page.tab.ln_info[s].x = 0;
        g_svg_page.tab.ln_info[s].do_carriage_return = NULL;
    }


    svgtypesetter_refresh_fbrd_xy();
}

static void svgtypesetter_fclose(void) {
    char pn[20];

    if (g_svg_page.fp != NULL) {
        snprintf(pn, sizeof(pn) - 1, "-%d-", g_svg_page.page_nr);
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
    g_svg_page.tp = NULL;

    g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[0].x;
    g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[0].y;

    g_svg_page.tab.last_non_null_x = 0;

    g_svg_page.tab.last_symbol = kTlpNone;

    return 1;
}

static void svgtypesetter_spill_tab_notation(const tulip_single_note_ctx *song) {
    struct typesetter_curr_settings cset = typesetter_settings();
    tulip_command_t used_techniques[31];
    size_t used_techniques_nr = 0, u = 0;
    int has_muffled = 0, has_anyfret = 0;
    int hp_done = 0;

    if ((cset.prefs & kTlpPrefsIncludeTabNotation) == 0 || song == NULL) {
        return;
    }

    *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN + 2;

    get_all_used_techniques(song, used_techniques, &used_techniques_nr, &has_muffled, &has_anyfret);

    if (used_techniques_nr > 0 || has_muffled || has_anyfret) {
        for (u = 0; u < used_techniques_nr; u++) {
            *g_svg_page.tab.carriage_x = g_svg_page.tab.xlim_left + 20;
            switch (used_techniques[u]) {
                case kTlpHammerOn:
                case kTlpPullOff:
                    if (!hp_done) {
                        hp_done = 1;
                        *g_svg_page.tab.carriage_y -= 4;
                        svgtypesetter_flush_hammer_on_pull_off_pinch();
                        *g_svg_page.tab.carriage_y += 4;
                        svgtypesetter_hammer_on_xstep(1);
                        fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                               " font-size=\"11\" font-family=\"Courier\">............ %s/%s"
                                               "</text>\n", *g_svg_page.tab.carriage_x,
                                                            *g_svg_page.tab.carriage_y,
                                                            get_technique_notation_label(kTlpHammerOn),
                                                            get_technique_notation_label(kTlpPullOff));
                    }
                    break;

                case kTlpVibrato:
                    svgtypesetter_flush_vibrato_pinch();
                    svgtypesetter_vibrato_xstep(1);
                    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                           " font-size=\"11\" font-family=\"Courier\"> ............ %s"
                                           "</text>\n", *g_svg_page.tab.carriage_x + 10,
                                                        *g_svg_page.tab.carriage_y,
                                                        get_technique_notation_label(kTlpVibrato));
                    break;

                case kTlpBend:
                    *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN * 2;
                    svgtypesetter_flush_bend_pinch(1);
                    svgtypesetter_bend_xstep(1);
                    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                           " font-size=\"11\" font-family=\"Courier\"> ............ %s"
                                           "</text>\n", *g_svg_page.tab.carriage_x + 14,
                                                        *g_svg_page.tab.carriage_y + ((u == 0) ? 0 :
                                                                           -SVGTYPESETTER_TAB_Y_SPAN/2),
                                                        get_technique_notation_label(kTlpBend));
                    break;

                case kTlpReleaseBend:
                    svgtypesetter_flush_release_bend_pinch(1);
                    svgtypesetter_release_bend_xstep(1);
                    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                           " font-size=\"11\" font-family=\"Courier\"> ............ %s"
                                           "</text>\n", *g_svg_page.tab.carriage_x + 14,
                                                        (*g_svg_page.tab.carriage_y) + ((u == 0) ? 0 :
                                                                           SVGTYPESETTER_TAB_Y_SPAN/2),
                                                        get_technique_notation_label(kTlpReleaseBend));
                    break;

                case kTlpSlideUp:
                    *g_svg_page.tab.carriage_y -= 4;
                    svgtypesetter_flush_slide_up_pinch();
                    *g_svg_page.tab.carriage_y += 4;
                    svgtypesetter_slide_up_xstep(1);
                    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                           " font-size=\"11\" font-family=\"Courier\"> ............ %s"
                                           "</text>\n", *g_svg_page.tab.carriage_x + 15,
                                                        *g_svg_page.tab.carriage_y,
                                                        get_technique_notation_label(kTlpSlideUp));
                    break;

                case kTlpSlideDown:
                    *g_svg_page.tab.carriage_y -= 4;
                    svgtypesetter_flush_slide_down_pinch();
                    *g_svg_page.tab.carriage_y += 4;
                    svgtypesetter_slide_down_xstep(1);
                    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                           " font-size=\"11\" font-family=\"Courier\"> ............ %s"
                                           "</text>\n", *g_svg_page.tab.carriage_x + 15,
                                                        *g_svg_page.tab.carriage_y,
                                                        get_technique_notation_label(kTlpSlideDown));
                    break;


                default:
                    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                   " font-size=\"11\" font-family=\"Courier\">%s"
                                   "</text>\n", *g_svg_page.tab.carriage_x,
                                                *g_svg_page.tab.carriage_y, get_technique_label(used_techniques[u]));
                    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                           " font-size=\"11\" font-family=\"Courier\"> ............ %s"
                                           "</text>\n", *g_svg_page.tab.carriage_x + 25,
                                                        *g_svg_page.tab.carriage_y,
                                                        get_technique_notation_label(used_techniques[u]));
                    break;
            }

            *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN + 3;
        }

        *g_svg_page.tab.carriage_x = g_svg_page.tab.xlim_left + 20;

        if (has_muffled) {
            fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                   " font-size=\"11\" font-family=\"Courier\">X"
                                   "</text>\n", *g_svg_page.tab.carriage_x,
                                                *g_svg_page.tab.carriage_y);
            fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                   " font-size=\"11\" font-family=\"Courier\"> ............ Muffled note"
                                   "</text>\n", *g_svg_page.tab.carriage_x + 30,
                                                *g_svg_page.tab.carriage_y);
            *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN + 3;
        }

        if (has_anyfret) {
            fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                   " font-size=\"11\" font-family=\"Courier\">?"
                                   "</text>\n", *g_svg_page.tab.carriage_x,
                                                *g_svg_page.tab.carriage_y);
            fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                   " font-size=\"11\" font-family=\"Courier\"> ............ From any fret"
                                   "</text>\n", *g_svg_page.tab.carriage_x + 30,
                                                *g_svg_page.tab.carriage_y);
            *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN + 3;
        }
    }

    svgtypesetter_refresh_fbrd_xy();
}

static void svgtypesetter_spill_tuning(void) {
    struct typesetter_curr_settings cset = typesetter_settings();
    char **tuning;
    size_t tuning_nr;
    ssize_t t;

    if ((cset.prefs & kTlpPrefsShowTuning) && !(cset.prefs & kTlpPrefsAddTuningToTheFretboard)) {
        *g_svg_page.tab.carriage_x = g_svg_page.tab.xlim_left;

        *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN * 2;

        tuning = get_processor_setting("tuning", &tuning_nr);
        fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"13\""
                               " font-weight=\"bold\">Tuning [%d-1]:</text>\n", *g_svg_page.tab.carriage_x,
                                                                                *g_svg_page.tab.carriage_y,
                                                                                tuning_nr);

        fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"13\">", *g_svg_page.tab.carriage_x + 78,
                                                                              *g_svg_page.tab.carriage_y,
                                                                              tuning_nr);

        for (t = tuning_nr - 1; t >= 0; t--) {
            fprintf(g_svg_page.fp, "%s%s", tuning[t], (t >  0) ? ", " : ".");
        }

        fprintf(g_svg_page.fp, "</text>\n");

        svgtypesetter_refresh_fbrd_xy();
    }
}

static void svgtypesetter_clean_old_pages_not_rewritten(void) {
    do {
        snprintf(g_svg_page.curr_pagefile, sizeof(g_svg_page.curr_pagefile) - 1, "%s-%03d.svg", g_svg_page.filename,
                                                                                                g_svg_page.page_nr++);
    } while (remove(g_svg_page.curr_pagefile) == 0);
}
