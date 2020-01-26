/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/svg/svginkspill.h>
#include <processor/typesetters/svg/svgboundaries.h>
#include <processor/typesetters/svg/svg_do_pinch_flush.h>
#include <processor/typesetters/typeprefs.h>
#include <processor/oututils.h>
#include <processor/settings.h>
#include <base/types.h>
#include <base/memory.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

// TODO(Rafael): Strip off all deprecated code.

// TIP(Rafael): 'Pinch', 'pinched' here is a lousy pun for 'punched' from punched cards ;)
//              When reading the word 'TAB', please do not scream, I am just using the standard way of write it here.

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
    // INFO(Rafael): Flags if a carriage return is sched or not.
    int sched_cr;
    // INFO(Rafael): Points to the current processed offset in current TAB diagram,
    //               it stands for the 'physical' section area of the TAB diagram currently under typesetting process.
    int *offset;
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

static inline void svgtypesetter_hammer_on_xstep(const int dit);

static inline void svgtypesetter_vibrato_xstep(const int dit);

static inline void svgtypesetter_pull_off_xstep(const int dit);

static inline void svgtypesetter_release_bend_xstep(const int dit);

static inline void svgtypesetter_bend_xstep(const int dit);

static inline void svgtypesetter_note_sep_xstep(const int dit);

static inline void svgtypesetter_slide_down_xstep(const int dit);

static inline void svgtypesetter_slide_up_xstep(const int dit);

static inline void svgtypesetter_sep_bar_xstep(const int dit);

static inline void svgtypesetter_chord_span_xstep(const int dit);

static inline void svgtypesetter_user_note_span_xstep(const int dit);

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

static int svgtypesetter_is_chord(const char **strings, const size_t offset, const size_t max_offset);

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

static void svgtypesetter_move_carriage_to_best_fit(const int min_space_amount, const int schedule_cr);

static void svgtypesetter_carriage_return(const int space_amount, int *xreg);

static int svgtypesetter_has_sep(const char *strings, const size_t offset, const size_t fretboard_sz, const char sep);

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
    //
    //               Maybe you could find that generate the SVG would be easier by following the pseudo code generated when
    //               compiling the user's tulip script, pointed by song parameter, but it is not. Reading a well-normalized
    //               ascii TAB is much more straightforward than do demuxing all recursive tags and re-implement SVG printers
    //               based on those DSL statements.
    //
    //               In fact, all processor implemented until now takes advantage at some point of the txt processor's hard
    //               work. Excepting txt processor, the remaining ones could be considered a kind of high-level processors.

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

static int svgtypesetter_has_sep(const char *strings, const size_t offset, const size_t fretboard_sz, const char sep) {
    const char *sp, *sp_end;
    int has = 0;
    sp = strings + offset;
    sp_end = sp + strlen(sp);

    //printf("%d - %d\n", sp_end - sp, strlen(strings));

    while (sp != sp_end && !has) {
        has = (*sp == sep);
        sp++;
    }

    return has;
}

static void svgtypesetter_newtabdiagram(const txttypesetter_tablature_ctx *txttab) {
    int comments_nr = 0, s_techniques_nr = 0, has_times = 0, tab_auto_break = 0, y, y0, y1, has_bend_or_release_bend = 0;
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
    //               Bear in mind that ascii TAB assumes monospaced fonts and all technique symbols are represented
    //               by only one ascii symbol, while a SVG TAB will use non-ascii symbols for some techniques.

    tab_auto_break = (g_svg_page.tab.last_non_null_x >= (g_svgtypesetter_page_width - SVGTYPESETTER_TAB_X_SPAN));

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

    // INFO(Rafael): Here is being verifying if there is a bend or release bend that could overlap the new tab diagram.


    if (g_svg_page.tab.offset != NULL) {
        if (txttab->last == NULL && tab_auto_break) {
            has_bend_or_release_bend = svgtypesetter_has_sep(txttab->strings[0],
                                                             *g_svg_page.tab.offset, txttab->fretboard_sz, 'b') +
                                       svgtypesetter_has_sep(txttab->strings[5], 0, *g_svg_page.tab.offset, 'r');
        } else if (txttab->last != NULL) {
            has_bend_or_release_bend = svgtypesetter_has_sep(txttab->strings[0],
                                                             *g_svg_page.tab.offset, txttab->fretboard_sz, 'b') +
                                       svgtypesetter_has_sep(txttab->strings[5], 0, *g_svg_page.tab.offset, 'r') +
                                       svgtypesetter_has_sep(txttab->last->strings[5], 0, txttab->last->fretboard_sz, 'r');
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
                                  ((has_bend_or_release_bend > 0) ? SVGTYPESETTER_TAB_Y_SPAN * has_bend_or_release_bend : 0) +
                                  ((tab_auto_break) ? SVGTYPESETTER_TAB_Y_SPAN : 0) +
                                  ((s_techniques_nr > 0) ? SVGTYPESETTER_TAB_Y_SPAN * 2 : 0) +
                                  ((SVGTYPESETTER_TAB_Y_SPAN * 2) * s_techniques_nr) +
                                  ((SVGTYPESETTER_TAB_Y_SPAN + 2) * comments_nr + 10) +
                                  ((has_times) ? SVGTYPESETTER_TAB_Y_SPAN * 2 : 0);

    // INFO(Rafael): The refresh function considers the fbrd[0].y coordinate to do the relative update
    //               over remaining strings coordinates.

    g_svg_page.tab.fbrd[0].y = *g_svg_page.tab.carriage_y;
    svgtypesetter_refresh_fbrd_xy();

    if (g_svg_page.tab.fbrd[5].y >= (g_svgtypesetter_page_height - (SVGTYPESETTER_TAB_Y_SPAN * 6))) {
        // INFO(Rafael): The current page became full, we need a new one.
        svgtypesetter_newpage();
    } else if (tab_auto_break) {
        // INFO(Rafael): When a auto break occurs, sometimes it can result in an overlap. Let's try to
        //               mitigate it.
        y = g_svg_page.tab.fbrd[0].y;
        if (s_techniques_nr > 0) {
            y -= (SVGTYPESETTER_TAB_Y_SPAN * 2) - 10;
        }

        if (y >= y0 && y <= y1) {
            // INFO(Rafael): The heuristic space was not so good we still have an overlapping.
            y = y1 + SVGTYPESETTER_TAB_Y_SPAN + ((s_techniques_nr == 0) ? 5 : 0);
        }
        g_svg_page.tab.fbrd[0].y = y;
        svgtypesetter_refresh_fbrd_xy();

        if (g_svg_page.tab.fbrd[5].y >= (g_svgtypesetter_page_height - (SVGTYPESETTER_TAB_Y_SPAN * 6))) {
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

    // INFO(Rafael): This function handles the .literal tag statements present in each TAB diagram.
    //               Here we will actually written them down to the SVG stream.

    if (txttab->last == NULL || g_svg_page.tab_per_page_nr == 0) {
        // INFO(Rafael): It is, in fact, the first one TAB diagram. It means no data will written
        //               until now. So we can use fbrd[0].y as reference.
        y = g_svg_page.tab.fbrd[0].y;
    } else {
        // INFO(Rafael): At least one TAB diagram was written.
        if (has_unflushed_data((const char **)txttab->last->strings, 0, txttab->last->fretboard_sz)) {
            // INFO(Rafael): Non empty TAB.
            y = g_svg_page.tab.fbrd[5].y;
            y += (SVGTYPESETTER_TAB_Y_SPAN + 2) * 2;
        } else {
            // INFO(Rafael): The last one was an empty TAB, probably generated by a '.literal{"Lipsum"};'.
            y = g_svg_page.tab.fbrd[0].y;
            y += SVGTYPESETTER_TAB_Y_SPAN + 2;
        }
        if (y >= (g_svgtypesetter_page_height - (SVGTYPESETTER_TAB_Y_SPAN * 6))) {
            // INFO(Rafael): The current page became full, we need a new one. Since we are adding y, we need to check
            //               it against the page vertical boundaries.
            svgtypesetter_newpage();
            y = g_svg_page.tab.fbrd[0].y;
        }
    }

    // INFO(Rafael): Now, knowing the initial value of the first comment y we can written the first commentary
    //               and derive the subsequent y for writing them too.

    cp = txttab->comments;
    p = lp = cp->data;
    p_end = p + strlen(p);

    while (cp != NULL) {
        // INFO(Rafael): However, .literal tag statements supports new line feed through '\n'. In SVG output
        //               a line feed will generate a new <text>...</text> section, what virtually tokenize
        //               .literal{"foo\nbar"} into two .literals without the explicity line feeding.
        while (lp != p_end && *lp != '\n') {
            lp++;
        }
        memset(comment, 0, sizeof(comment));
        if (p != NULL) {
            memcpy(comment, p, lp - p);
        }
        p = lp + 1;
        if (strlen(comment) > 0) {
            fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" fill=\"black\" font-size=\"11\" font-family=\"Courier\">"
                                   "%s</text>\n", g_svg_page.tab.xlim_left,
                                                  y,
                                                  comment);
        }
        y += SVGTYPESETTER_TAB_Y_SPAN + 2;
        if (y >= (g_svgtypesetter_page_height - (SVGTYPESETTER_TAB_Y_SPAN * 6))) {
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
        // INFO(Rafael): A new TAB diagram will actually drawn into current SVG stream.
        g_svg_page.tab.fbrd[0].y = y;
        svgtypesetter_refresh_fbrd_xy();
    }
}

// TIP(Rafael): A function named as 'technique_xstep' stands for a horizontal carriage stepper of a specific tablature symbol.
//              All stepper x increase amounts are heuristic. The int dit received parameter is an acronym which stands for
//              [di]rection and [t]imes, where the signal of this integer gives the horizontal direction and the absolute
//              value gives how many times the x carriage must walk. In fact the xstep functions take advantage of the
//              implicit cartesian plane concept present in SVG coordinate system. Thus, by passing 1 means '- go one step
//              ahead' and passing -1 means '- go one step back'.

static inline void svgtypesetter_hammer_on_xstep(const int dit) {
    *g_svg_page.tab.carriage_x += (SVGTYPESETTER_TAB_X_SPAN + 10) * dit;
}

static inline void svgtypesetter_vibrato_xstep(const int dit) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_TAB_X_SPAN * dit;
}

static inline void svgtypesetter_pull_off_xstep(const int dit) {
    svgtypesetter_hammer_on_xstep(dit);
}

static inline void svgtypesetter_release_bend_xstep(const int dit) {
    svgtypesetter_bend_xstep(dit);
}

static inline void svgtypesetter_bend_xstep(const int dit) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_TAB_X_SPAN * dit;
}

static inline void svgtypesetter_note_sep_xstep(const int dit) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_TAB_X_SPAN * dit;
}

static inline void svgtypesetter_sep_bar_xstep(const int dit) {
    *g_svg_page.tab.carriage_x += (SVGTYPESETTER_TAB_X_SPAN - 5) * dit;
}

static inline void svgtypesetter_slide_down_xstep(const int dit) {
    *g_svg_page.tab.carriage_x += (SVGTYPESETTER_TAB_X_SPAN - 5) * dit;
}

static inline void svgtypesetter_slide_up_xstep(const int dit) {
    svgtypesetter_slide_down_xstep(dit);
}

static inline void svgtypesetter_chord_span_xstep(const int dit) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_CHORD_SPAN_X_STEP * dit;
}

static inline void svgtypesetter_user_note_span_xstep(const int dit) {
    *g_svg_page.tab.carriage_x += SVGTYPESETTER_USER_NOTE_SPAN_X_STEP;
}

static void svgtypesetter_insert_header_span(void) {
    // INFO(Rafael): Until now this function is responsible for inserting a header blank span between header section and
    //               the first TAB diagram.
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

static void svgtypesetter_carriage_return(const int space_amount, int *xreg) {
    // INFO(Rafael): This function performs a carriage return based on the last effective used x coordinate. An
    //               effective used coordinate means a coordinate where a note sep different of '-' was inserted.
    //               With it is possible to allow user to increase the default spacing between notes by using periods of
    //               single dashes, e.g. '10-----45', without screwing up the typesetting of separators that must
    //               not be more spaced than the previous defined spacing amount.
    //
    //              space_amount: how many pixels must be added after doing the return.
    //              xreg: it saves the current carriage x coordinate for later processing by the caller.
    if (xreg != NULL) {
        *xreg = *g_svg_page.tab.carriage_x;
    }
    *g_svg_page.tab.carriage_x = g_svg_page.tab.curr_ln_info->x + space_amount;
}

// TIP(Rafael): Functions named as 'flush_technique' stands for specialized pinch functions. Those functions are
//              responsible for writing/drawing symbols relative to notes and techniques on TAB diagram.

static void svgtypesetter_flush_note_pinch(const char *note) {
    const char *np = note;
    int xreg = 0;

    if (g_svg_page.tab.curr_ln_info->do_carriage_return != NULL && g_svg_page.tab.curr_ln_info->x > 0) {
        // INFO(Rafael): If do_carriage_return was scheduled we need to perform this return before
        //               pinching the current note on TAB diagram. The current carriage x value will
        //               be restored after the note pinching, so we need to pass xreg's address.
        g_svg_page.tab.curr_ln_info->do_carriage_return(g_svg_page.tab.curr_ln_info->do_cr_arg, &xreg);
    }

    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"13\""
                           " font-weight=\"bold\" font-family=\"Courier\">", *g_svg_page.tab.carriage_x,
                                                                             *g_svg_page.tab.carriage_y +
                                                                             SVGTYPESETTER_TAB_NOTE_Y_OFFSET);
    if (xreg > 0) {
        *g_svg_page.tab.carriage_x = xreg;
    }

    // INFO(Rafael): If your fretboard generates notes more than 99 in a tulip script, I am sorry.
    while ((isdigit(*np) || *np == 'X' || *np == '?') && (np - note) < 2) {
        fprintf(g_svg_page.fp, "%c", *np);
        np++;
    }
    g_svg_page.tab.curr_ln_info->len = (np - note);
    fprintf(g_svg_page.fp, "</text>\n");
}

static void svgtypesetter_flush_bend_pinch(const int arrow) {
    // DEPRECATED(Rafael): Nice but it would add rendering limitations.
    /*
    char *arrow_markup = (arrow) ? " marker-end=\"url(#arrow)\"" : "";
    svgtypesetter_move_carriage_to_best_fit(7, 0);
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
    */
    svgtypesetter_move_carriage_to_best_fit(SVGTYPESETTER_BEND_MIN_SPACE, 0);
    fprintf(g_svg_page.fp, "\t<path d=\"M%d,%d C%d,%d, %d,%d %d,%d\""
                           " fill=\"none\" stroke=\"black\" stroke-width=\"1\"/>\n", *g_svg_page.tab.carriage_x,
                                                                                       *g_svg_page.tab.carriage_y,
                                                                                       *g_svg_page.tab.carriage_x,
                                                                                       *g_svg_page.tab.carriage_y + 5,
                                                                                       *g_svg_page.tab.carriage_x + 5,
                                                                                       *g_svg_page.tab.carriage_y + 5,
                                                                                       *g_svg_page.tab.carriage_x + 5,
                                                                                       *g_svg_page.tab.carriage_y - 15);
    if (arrow) {
        // INFO(Rafael): For a best support let's avoid using marker-end point to a location inside the own file,
        //               because certain viewers like gwenview 4.14 does not have support for this kind of location
        //               feature and I use it for my own stuff sometimes. Here we will explicitly draw a triangle at
        //               the end of the drawn curve.
        fprintf(g_svg_page.fp, "\t<path d=\"M%d,%d L%d,%d L%d,%d z\" fill=\"black\"/>\n", *g_svg_page.tab.carriage_x + 5,
                                                                                          *g_svg_page.tab.carriage_y - 23,
                                                                                          *g_svg_page.tab.carriage_x + 1,
                                                                                          *g_svg_page.tab.carriage_y - 15,
                                                                                          *g_svg_page.tab.carriage_x + 9,
                                                                                          *g_svg_page.tab.carriage_y - 15);
    }
}

static void svgtypesetter_flush_release_bend_pinch(const int arrow) {
    // DEPRECATED(Rafael): Nice but it would add rendering limitations.
    /*
    char *arrow_markup = (arrow) ? " marker-end=\"url(#arrow)\"" : "";
    svgtypesetter_move_carriage_to_best_fit(7, 0);
    fprintf(g_svg_page.fp, "\t<path d=\"M%d,%d C%d,%d %d,%d %d,%d\""
                           " fill=\"none\" stroke=\"black\" stroke-width=\"1\"%s/>\n", *g_svg_page.tab.carriage_x,
                                                                                       *g_svg_page.tab.carriage_y,
                                                                                       *g_svg_page.tab.carriage_x,
                                                                                       *g_svg_page.tab.carriage_y - 10,
                                                                                       *g_svg_page.tab.carriage_x + 5,
                                                                                       *g_svg_page.tab.carriage_y - 10,
                                                                                       *g_svg_page.tab.carriage_x + 4,
                                                                                       *g_svg_page.tab.carriage_y + 5,
                                                                                       arrow_markup);*/
    svgtypesetter_move_carriage_to_best_fit(SVGTYPESETTER_RELEASE_BEND_MIN_SPACE, 0);
    fprintf(g_svg_page.fp, "\t<path d=\"M%d,%d C%d,%d %d,%d %d,%d\""
                           " fill=\"none\" stroke=\"black\" stroke-width=\"1\"/>\n", *g_svg_page.tab.carriage_x,
                                                                                     *g_svg_page.tab.carriage_y,
                                                                                     *g_svg_page.tab.carriage_x,
                                                                                     *g_svg_page.tab.carriage_y - 10,
                                                                                     *g_svg_page.tab.carriage_x + 5,
                                                                                     *g_svg_page.tab.carriage_y - 10,
                                                                                     *g_svg_page.tab.carriage_x + 4,
                                                                                     *g_svg_page.tab.carriage_y + 5);

    if (arrow) {
        // INFO(Rafael): For a best support let's avoid using marker-end point to a location inside the own file,
        //               because certain viewers like gwenview 4.14 does not have support for this kind of location
        //               feature and I use it for my own stuff sometimes. Here we will explicitly draw a triangle at
        //               the end of the drawn curve.
        fprintf(g_svg_page.fp, "\t<path d=\"M%d,%d L%d,%d L%d,%d z\" fill=\"black\"/>\n", *g_svg_page.tab.carriage_x,
                                                                                          *g_svg_page.tab.carriage_y + 4,
                                                                                          *g_svg_page.tab.carriage_x + 4,
                                                                                          *g_svg_page.tab.carriage_y + 12,
                                                                                          *g_svg_page.tab.carriage_x + 9,
                                                                                          *g_svg_page.tab.carriage_y + 4);
    }
}

static void svgtypesetter_move_carriage_to_best_fit(const int min_space_amount, const int schedule_cr) {
    // INFO(Rafael): This function moves the x carriage to a "best fit" defined by
    //               the minimum space (pixels) between current x and last effective x.
    //               It finds for this best fit.
    int ml;
    if (g_svg_page.tab.curr_ln_info == NULL || g_svg_page.tab.curr_ln_info->x == 0) {
        return;
    }
    ml = min_space_amount * g_svg_page.tab.curr_ln_info->len;
    if ((*g_svg_page.tab.carriage_x - g_svg_page.tab.curr_ln_info->x) > ml) {
        do {
            *g_svg_page.tab.carriage_x -= 1;
        } while ((*g_svg_page.tab.carriage_x - g_svg_page.tab.curr_ln_info->x) > ml);
    }
    if (schedule_cr) {
        // INFO(Rafael): Let's schedule a carriage return. It certainly will be done before next note pinching.
        g_svg_page.tab.curr_ln_info->do_carriage_return = svgtypesetter_carriage_return;
        g_svg_page.tab.curr_ln_info->do_cr_arg = min_space_amount;
    }
}

static void svgtypesetter_flush_vibrato_pinch(void) {
    svgtypesetter_move_carriage_to_best_fit(SVGTYPESETTER_VIBRATO_MIN_SPACE, 0);
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"18\""
                           " font-family=\"Courier\">~</text>\n", *g_svg_page.tab.carriage_x,
                                                                  *g_svg_page.tab.carriage_y);
}

static void svgtypesetter_flush_hammer_on_pull_off_pinch(void) {
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"22\""
                           " transform=\"rotate(90,%d,%d)\" font-family=\"Courier\">(</text>\n", *g_svg_page.tab.carriage_x,
                                                                                                 *g_svg_page.tab.carriage_y - 3,
                                                                                                 *g_svg_page.tab.carriage_x + 4,
                                                                                                 *g_svg_page.tab.carriage_y - 3);
}

static void svgtypesetter_flush_slide_down_pinch(void) {
    svgtypesetter_move_carriage_to_best_fit(SVGTYPESETTER_SLIDE_DOWN_MIN_SPACE, g_svg_page.tab.sched_cr);
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"18\""
                           " font-family=\"Courier\">/</text>\n", *g_svg_page.tab.carriage_x,
                                                                  *g_svg_page.tab.carriage_y +
                                                                  SVGTYPESETTER_TAB_NOTE_Y_OFFSET + 2);
}

static void svgtypesetter_flush_slide_up_pinch(void) {
    svgtypesetter_move_carriage_to_best_fit(SVGTYPESETTER_SLIDE_UP_MIN_SPACE, g_svg_page.tab.sched_cr);
    fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"18\""
                           " font-family=\"Courier\">\\</text>\n", *g_svg_page.tab.carriage_x,
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

static int svgtypesetter_is_chord(const char **strings, const size_t offset, const size_t max_offset) {
    // INFO(Rafael): If this function finds at least two notes or techniques stacked, it will consider the passed TAB snippet
    //               as being a chord.
    size_t s;
    int is = 0;

    if (strings == NULL || *strings == 0) {
        return 0;
    }

    for (s = 0; s < 6 && is < 2; s++) {
        is += (strings[s][offset] != '-');
    }

    if (is == 1 && (offset + 1) < max_offset) {
        for (s = 0; s < 6 && is < 2; s++) {
            is += (strings[s][offset + 1] != '-');
        }
    }

    return (is == 2);
}

static size_t svgtypesetter_get_release_bend_arrow_string(const char **strings, const size_t offset) {
    // INFO(Rafael): This function finds the string number that must contain release bend indication headed by a arrow.
    //               It is handy if you consider bends over internals/double stops. Return 6 means no release bends
    //               for this TAB snippet.
    ssize_t s;

    for (s = 5; s >= 0; s--) {
        if (strings[s][offset] == 'r') {
            return (size_t)s;
        }
    }

    return 6;
}

static size_t svgtypesetter_get_bend_arrow_string(const char **strings, const size_t offset) {
    // INFO(Rafael): This function finds the string number that must contain a bend indication headed by a arrow.
    //               It is handy if you consider bends over internals/double stops. Return 6 means no bends for this
    //               TAB snippet.
    size_t s;

    for (s = 0; s < 6; s++) {
        if (strings[s][offset] == 'b') {
            return s;
        }
    }

    return 6;
}

static void svgtypesetter_reduce_blank_yspan(const txttypesetter_tablature_ctx *txttab) {
    // DEPRECATED(Rafael): wtf...
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
    // INFO(Rafael): This function verifies if from current TAB snippet to its end has something different
    //               of the ordinary note sep '-'. Thus, by passing offset 0 means verify if the current
    //               ascii TAB diagram is an empty one or not.
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
    size_t s, offset, s_temp;
    int has_bad_align, is_chord;
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

    // INFO(Rafael): Dirty trick. Here we will undo any fancier chord alignment done by txt typesetter.
    //
    //               Thus, considering a chord alignment looking like:
    //
    //               |-------...
    //               |-------...
    //               |---7---...
    //               |---9---...
    //               |--10---...
    //               |-------...
    //
    //               It will become uglier, looking like:
    //
    //               |-------...
    //               |-------...
    //               |--7----...
    //               |--9----...
    //               |--10---...
    //               |-------...
    //
    //               Fancier chord alignments came from txt typesetting could confuse the SVG typesetter
    //               making it produce ugly alignments. The following for-loop is only for mitigating
    //               this possibility. Resuming, we screw it up by now, for making it good again later.

    for (offset = 0; offset < txttab->fretboard_sz; offset++) {
        if (svgtypesetter_is_chord((const char **)txttab->strings, offset, txttab->fretboard_sz)) {
            for (s = 0; s < 6; s++) {
                if (txttab->strings[s][offset] == '-' && isdigit(txttab->strings[s][offset + 1])) {
                    txttab->strings[s][offset] = txttab->strings[s][offset + 1];
                    txttab->strings[s][offset + 1] = '-';
                }
            }

            while (svgtypesetter_is_chord((const char **)txttab->strings, offset++, txttab->fretboard_sz))
                ;
            offset--;

        }
    }


    /*for (s = 0; s < 6; s++) {
        printf("'%s'\n", txttab->strings[s]);
    }
    printf("\n");
    system("read");*/
}

static int svgtypesetter_refresh_fbrd_xy(void) {
    // INFO(Rafael): This function refreshes all fbrd coordinates for the current pointed value carriage_x and
    //               fbrd[0].y value. My choice for not using carriage_y is for the sake of a more foreseeable
    //               behavior of the refreshing process.
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
    // INFO(Rafael): This function closes the TAB more right by adding a vertical bar and erasing all
    //               remaining empty TAB diagram. Notice that "erasing" here is a dirty trick of drawing
    //               an white rect by overlapping all excedent TAB diagram area.
    size_t s;

    if (g_svg_page.fp == NULL) {
        return;
    }

    if (g_svg_page.tab.last_symbol != kTlpSepBar) {
        // INFO(Rafael): The last symbol drawn was not a sep bar, thus we need to draw one.
        svgtypesetter_flush_sep_bar();
    } else {
        // INFO(Rafael): A sep bar have been just drawn, now all we need to do is erasing TAB's empty area.
        *g_svg_page.tab.carriage_x = g_svg_page.tab.ln_info[0].x;
    }

    fprintf(g_svg_page.fp, "\t<rect x=\"%d\" y=\"%d\" width=\"%d\""
                           " height=\"%d\" fill=\"white\"/>\n", *g_svg_page.tab.carriage_x + 1,
                                                                g_svg_page.tab.fbrd[0].y - SVGTYPESETTER_TAB_Y_SPAN,
                                                                g_svgtypesetter_page_width - *g_svg_page.tab.carriage_x + 1,
                                                                SVGTYPESETTER_TAB_Y_SPAN * 6 + SVGTYPESETTER_TAB_Y_SPAN / 2);
}

static int svgtypesetter_newpage(void) {
    // INFO(Rafael): It asks for a new page by closing the current file descriptor and resetting all coordinate values.
    //               A new page involves to create the next numbered file based on the filename root supplied by the user.

    size_t s;

    // DEPRECATED(Rafael): wtf...
    /*if (g_svg_page.tp != NULL && g_svg_page.tp->last != NULL &&
        has_unflushed_data((const char **)g_svg_page.tp->last->strings, 0, g_svg_page.tp->last->fretboard_sz)) {
        svgtypesetter_cut_tab();
    }*/

    if (g_svg_page.fp != NULL) {
        svgtypesetter_fclose();
    }

    snprintf(g_svg_page.curr_pagefile, sizeof(g_svg_page.curr_pagefile) - 1, "%s-%03d.svg", g_svg_page.filename,
                                                                                            g_svg_page.page_nr);

    if ((g_svg_page.fp = fopen(g_svg_page.curr_pagefile, "w")) == NULL) {
        return 0;
    }

    g_svg_page.tab.xlim_left = SVGTYPESETTER_TAB_XL_DELTA;
    g_svg_page.tab.xlim_right = g_svgtypesetter_page_width + SVGTYPESETTER_TAB_XR_DELTA;

    g_svg_page.tab.fbrd[0].x = g_svg_page.tab.xlim_left;
    g_svg_page.tab.fbrd[0].y = SVGTYPESETTER_TAB_Y;

    g_svg_page.tab.last_carriage_x = g_svg_page.tab.carriage_x;
    g_svg_page.tab.last_carriage_y = g_svg_page.tab.carriage_y;

    g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[0].x;
    g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[0].y;

    for (s = 0; s < 6; s++) {
        // INFO(Rafael): If all x was reseted, we also need to reset the last note x info.
        g_svg_page.tab.ln_info[s].x = *g_svg_page.tab.carriage_x;
        // INFO(Rafael): Nothing was really 'pinched' on this new diagram, on this string, so let's set it to zero.
        g_svg_page.tab.ln_info[s].len = 0;
    }

    svgtypesetter_refresh_fbrd_xy();

    // DEPRECATED(Rafael): Seeking a wide support we will not use end-makers anymore.
    /*fprintf(g_svg_page.fp, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>\n"
                           "<svg xmlns=\"http://www.w3.org/2000/svg\""
                           " width=\"%d\" height=\"%d\" style=\"background-color:white\">\n"
                           "\t<rect x=\"1\" y=\"1\" width=\"%d\" height=\"%d\" fill=\"white\"/>\n"
                           "\t<defs>\n"
                            "\t\t<marker id=\"arrow\" markerWidth=\"10\" markerHeight=\"10\" refX=\"1\" refY=\"4\""
                            " orient=\"auto\" markerUnits=\"strokeWidth\">\n"
                            "\t\t\t<path d=\"M0,0 L0,8 L9,3 z\" fill=\"black\"/>\n"
                            "\t\t</marker>\n"
                            "\t</defs>\n", SVGTYPESETTER_PAGE_WIDTH, SVGTYPESETTER_PAGE_HEIGHT,
                                           SVGTYPESETTER_PAGE_WIDTH, SVGTYPESETTER_PAGE_HEIGHT);*/
    fprintf(g_svg_page.fp, "<?xml version=\"1.0\" encoding=\"%s\" standalone=\"no\"?>\n"
                           "<svg xmlns=\"http://www.w3.org/2000/svg\""
                           " width=\"%d\" height=\"%d\" style=\"background-color:white\">\n"
                           "\t<rect x=\"1\" y=\"1\" width=\"%d\" height=\"%d\" fill=\"white\"/>\n",
                           g_svgtypesetter_svg_encoding, g_svgtypesetter_page_width,
                           g_svgtypesetter_page_height, g_svgtypesetter_page_width,
                           g_svgtypesetter_page_height, g_svgtypesetter_svg_encoding);

    g_svg_page.tab_per_page_nr = 1;

    g_svg_page.tab.carriage_x = g_svg_page.tab.last_carriage_x;
    g_svg_page.tab.carriage_y = g_svg_page.tab.last_carriage_y;

    return 1;
}

static void svgtypesetter_spill_tabdiagram(void) {
    // INFO(Rafael): This function effectively draws a TAB diagram into the SVG stream, taking into consideration the
    //               current master y axis of the current page/image.

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
                                                                                          g_svg_page.tab.fbrd[0].y +
                                                                                            SVGTYPESETTER_TAB_Y_SPAN * 5);
    g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[0].x;
    *g_svg_page.tab.carriage_x = x + SVGTYPESETTER_TAB_X_SPAN;

    for (s = 0; s < 6; s++) {
        g_svg_page.tab.ln_info[s].x = 0;
        g_svg_page.tab.ln_info[s].do_carriage_return = NULL;
    }


    svgtypesetter_refresh_fbrd_xy();
}

static void svgtypesetter_fclose(void) {
    // INFO(Rafael): This function closes the current SVG stream related to the page but add its numbering before actually
    //               closing it.
    char pn[20];

    if (g_svg_page.fp != NULL) {
        snprintf(pn, sizeof(pn) - 1, "-%d-", g_svg_page.page_nr);
        fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" fill=\"black\""
                               " font-size=\"13\" font-family=\"Courier\">%s</text>\n", g_svgtypesetter_page_width / 2 -
                                                                                            (strlen(pn) - 1),
                                                                                        g_svgtypesetter_page_height -
                                                                                        SVGTYPESETTER_TAB_Y_SPAN,
                                                                                        pn);
        fprintf(g_svg_page.fp, "</svg>\n");
        fclose(g_svg_page.fp);
        g_svg_page.fp = NULL;
        g_svg_page.page_nr++;
    }
}

static int svgtypesetter_init(const char *filename) {
    // INFO(Rafael): Initialisation function for g_svg_page global module struct.
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
    g_svg_page.tab.offset = NULL;

    g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[0].x;
    g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[0].y;

    g_svg_page.tab.last_non_null_x = 0;

    g_svg_page.tab.last_symbol = kTlpNone;

    return 1;
}

static void svgtypesetter_spill_tab_notation(const tulip_single_note_ctx *song) {
    //INFO(Rafael): It writes the TAB notation based on all used techniques present in tulip script. Here is being used a
    //              monospaced font in order to seek a more a fancy alignment.
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
    // INFO(Rafael): It writes to SVG the configured tuning.
    struct typesetter_curr_settings cset = typesetter_settings();
    char **tuning;
    size_t tuning_nr;
    ssize_t t;

    if ((cset.prefs & kTlpPrefsShowTuning) && !(cset.prefs & kTlpPrefsAddTuningToTheFretboard)) {
        *g_svg_page.tab.carriage_x = g_svg_page.tab.xlim_left;

        *g_svg_page.tab.carriage_y += SVGTYPESETTER_TAB_Y_SPAN * 2;

        tuning = get_processor_setting("tuning", &tuning_nr);
        fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"13\""
                               " font-weight=\"bold\" font-family=\"Courier\">"
                               "Tuning [%d-1]:</text>\n", *g_svg_page.tab.carriage_x,
                                                          *g_svg_page.tab.carriage_y,
                                                          tuning_nr);

        fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\" font-size=\"13\""
                               " font-family=\"Courier\">", *g_svg_page.tab.carriage_x + 110,
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
    // INFO(Rafael): Since this processor divides generated pages into numbered SVG files, after processing the last page
    //               all remaining old files generated from a later processing are removed by this function.
    do {
        snprintf(g_svg_page.curr_pagefile, sizeof(g_svg_page.curr_pagefile) - 1, "%s-%03d.svg", g_svg_page.filename,
                                                                                                g_svg_page.page_nr++);
    } while (remove(g_svg_page.curr_pagefile) == 0);
}

static void svgtypesetter_flush_fretboard_pinches(txttypesetter_tablature_ctx *txttab) {
    // INFO(Rafael): Maybe this function be the toughest function of grasping into here in this implementation file.
    //               The most important thing of bearing in mind is that the function processes a TAB snippet per iteration.
    //               Each iteration goes from 0 up to the size in bytes of the ascii TAB diagram (its width).
    //
    //               At first glance you can imagine that the processing flow is based on traversing the TAB diagram as
    //               follows:
    //                          +---+          +----+
    //                          +   v          +    v
    //                          + |[-]---...   + |-[-]--...     |...--------|
    //                          + |------...   + |------...     |...--------|
    //                        0p+ |------... 1p+ |------... Np+ |...--------|
    //                            |------...     |------...   + |...--------|
    //                            |------...     |------...   + |...--------|
    //                            |------...     |------...   + |...-----[-]|
    //                                                        +-----------^
    //
    //                                                          where "Np" denotes the note processing iteration per string,
    //
    //               however, finding to seek a better typesetting in SVG output, the ascii TAB is processed in the following
    //               traverse strategy:
    //
    //                           0   1         N
    //                           p   p         p
    //                           +   +         +
    //                           v   v         v
    //                        | [-] [-] [...] [-] |
    //                        | [-] [-] [...] [-] |
    //                        | [-] [-] [...] [-] |
    //                        | [-] [-] [...] [-] |
    //                        | [-] [-] [...] [-] |
    //                        | [-] [-] [...] [-] |
    //
    //               This way of traversing the TAB notes will process all stacked notes at each iteration.
    //
    //               A vertical traversing will make easier to find fancier chord alignments, among other advantages.
    //               Opposingly, it will create some typesetting side-effects, too. Those side-effects need to be
    //               eliminated/compensated, otherwise the user will get a screwed-up SVG output at some point.

    txttypesetter_tablature_ctx *tp;
    size_t s, offset, null_nr = 0;
    void (*xstep)(const int) = NULL;
    void (*last_xstep)(const int) = NULL;
    struct notes_span_ctx {
        int is_beyond_9th_fret[6];
        int do_span;
    } notes_span;
    int bend_arrow_string, spill_done, is_chord;
    struct last_note_info_ctx ln_info;
    char *times, *times_end, tm_buf[20];
    struct sustained_techniques_points_ctx {
        int x;
        int y;
        char *data;
        char *data_end;
        int print_line;
    } stech_pts[20], *stech_p, *stech_end;
    txttypesetter_sustained_technique_ctx *sp;

    g_svg_page.tab.offset = &offset;

    for (tp = txttab; tp != NULL; tp = tp->next) {
        // INFO(Rafael): Each tp gathers comments, sustained techniques, times indications and TAB diagram.

        g_svg_page.tp = tp;

        if (tp->comments != NULL) {
            // INFO(Rafael): Write comments to the current SVG stream.
            svgtypesetter_spill_comments(tp);
        }

        if (!has_unflushed_data((const char **)tp->strings, 0, tp->fretboard_sz))  {
            // INFO(Rafael): If it was only a comment followed by a save-point, e.g.:
            //                            '.literal{"You got a monster in your parasol..."};'
            //               There is nothing to be done beyond here.
            continue;
        }

        // INFO(Rafael): Let's normalize it if necessary, in order to make fancy alignments, specially with chords
        //               that mixes frets before 10th position and from 10th position to higher.
        svgtypesetter_normalize_ascii_tab(tp);

        g_svg_page.tab.last_non_null_x = 0;

        // INFO(Rafael): This offset value will be used for the next TAB diagram.
        *g_svg_page.tab.offset = 0;

        // INFO(Rafael): We need a TAB diagram to start pinching notes and techniques.

        svgtypesetter_newtabdiagram(tp);

        // INFO(Rafael): The times pointer will traverse the buffer times when it is not NULL.

        times = tp->times;
        times_end = times + 3;

        if (tp->techniques != NULL) {
            // INFO(Rafael): The svgtypesetter_newtabdiagram() function also considered the space
            //               consumed by all sustained tecnhiques pilled up the TAB diagram.
            //               All you must do is to compute the exact y coordinate taking into
            //               consideration the fbrd[0].y, the first string in TAB diagram.
            stech_p = &stech_pts[0];
            stech_end = stech_p + sizeof(stech_pts) / sizeof(stech_pts[0]);
            sp = tp->techniques;
            while (stech_p != stech_end && sp != NULL) {
                // INFO(Rafael): Initializing each sustained technique info record.
                stech_p->x = *g_svg_page.tab.carriage_x;
                // INFO(Rafael): Each sustained technique has its own buffer. It will make simple to manage
                //               where start plotting the specific technique indication and where stopping.
                //               Anyway, in the same buffer we can have more than one techinque being sustained
                //               ahead. Fortunately it is indiferent for the approach followed here.
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
                // INFO(Rafael): Based on the first computed y the subsequent ones are derived.
                stech_p[0].y = stech_p[-1].y - SVGTYPESETTER_TAB_Y_SPAN;
                stech_p++;
            }
        } else {
            stech_p = stech_end = NULL;
        }

        // INFO(Rafael): The following for-loop implements the traversing idea introduced at the beginning of this function.

        for (offset = 0; offset < tp->fretboard_sz; offset++) {
            // INFO(Rafael): Spilling sustained techniques.
            if (stech_end != NULL) {
                for (stech_p = &stech_pts[0]; stech_p != stech_end; stech_p++) {
                    if (stech_p->data >= stech_p->data_end) {
                        // INFO(Rafael): This technique was fully represented in the current SVG stream.
                        //               There is nothing to do with it from now on.
                        continue;
                    }
                    if (!stech_p->print_line && isalpha(*stech_p->data)) {
                        // INFO(Rafael): We have found the start of the current sustained technique. It involves to
                        //               record the current carriage x, write its label to the SVG stream and flagging that
                        //               this specific technique indication needs a dashed line right below indicating
                        //               the whole section of it should be done during the song. With a recorded x and
                        //               greater x value in the future, drawing this line will be pretty easy.
                        stech_p->x = *g_svg_page.tab.carriage_x;
                        fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                               " font-size=\"13\" font-weight=\"bold\""
                                               " font-family=\"Courier\">%c%c%c</text>\n", stech_p->x,
                                                                                           stech_p->y,
                                                                                           stech_p->data[0],
                                                                                           stech_p->data[1],
                                                                                           stech_p->data[2]);
                        stech_p->print_line = 1;
                    } else if (stech_p->print_line && stech_p->data[0] == '.' && stech_p->data[1] != '.') {
                        // INFO(Rafael): We have found the end of the current sustained technique. Let's draw a single
                        //               dashed line and also recording the new x coordinate, if it is necessary.
                        fprintf(g_svg_page.fp, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\""
                                               " style=\"stroke:rgb(0,0,0);stroke-width:1;opacity:0.5\""
                                               " stroke-dasharray=\"5,5\"/>\n", stech_p->x, *g_svg_page.tab.carriage_x,
                                                                                stech_p->y + 1, stech_p->y + 1);
                        if ((stech_p->data + 2) < stech_p->data_end &&
                            stech_p->data[1] == ' ' && tp->strings[0][offset + 1] == '|' && stech_p->data[2] == '.') {
                            // INFO(Rafael): Ascii TAB do not wraps '|' with sustained lines, e.g:
                            //
                            //    l.r. ............. .........................
                            //
                            //  |-------------------|-------------------------
                            //  |-------------------|-------------------------
                            //  |-------------------|-------------------------
                            //  |-------------------|-------------------------
                            //  |-------------------|-------------------------
                            //  |-------------------|-------------------------
                            //
                            // SVG processor does, due to it if sep bar is found in the current ascii TAB section,
                            // it will interpreted as a false end. Completed later, because print_line will stay set.
                            //
                            stech_p->x = *g_svg_page.tab.carriage_x;
                        } else {
                            // INFO(Rafael): It is not a false end. It is end of the technique indication, actually.
                            stech_p->print_line = 0;
                        }
                    }
                    stech_p->data++;
                }
            }

            // INFO(Rafael): Spilling times.
            if (times != NULL && times > times_end && isdigit(*times)) {
                // INFO(Rafael): We have found a tag times indication. We need to find where this indication ends and
                //               write it out to SVG stream.
                for (times_end = times; *times_end != 'X'; times_end++)
                    ;
                memset(tm_buf, 0, sizeof(tm_buf));
                memcpy(tm_buf, times, times_end - times + 1);
                fprintf(g_svg_page.fp, "\t<text x=\"%d\" y=\"%d\""
                                       " font-size=\"13\" font-weight=\"bold\""
                                       " font-family=\"Courier\">%s</text>\n", *g_svg_page.tab.carriage_x,
                                                                               g_svg_page.tab.fbrd[0].y -
                                                                               SVGTYPESETTER_TAB_Y_SPAN, tm_buf);
                // INFO(Rafael): Once a time indication written, it is not good idea write future data above it. The
                //               TAB would become messy. Let's advance the carriage x and make all remaining strings of
                //               the diagram aligned to this new position.
                *g_svg_page.tab.carriage_x += SVGTYPESETTER_TAB_X_SPAN;
                svgtypesetter_refresh_fbrd_xy();
            }

            // INFO(Rafael): Now we are effectively start inspecting the current TAB diagram section.
            //               This section is created/given by shifting '(strings *) + offset' positions.

            is_chord = svgtypesetter_is_chord((const char **)tp->strings, offset, tp->fretboard_sz);

            memset(&notes_span, 0, sizeof(notes_span));

            if (is_chord) {
                // INFO(Rafael): The current section being a chord push us to store some info
                //               about pinched notes in ascii TAB to apply an alignment beautify later, if necessary.
                //               For more details take a look at 'svg_do_pinch_flush.h'.
                for (s = 0; s < 6; s++) {
                    notes_span.is_beyond_9th_fret[s] = (isdigit(tp->strings[s][offset]) &&
                                                        isdigit(tp->strings[s][offset + 1]));
                    if (!notes_span.do_span && notes_span.is_beyond_9th_fret[s]) {
                        notes_span.do_span = 1;
                    }
                    if (tp->strings[s][offset] == '-') {
                        // INFO(Rafael): This if-clause ensures that for example '.chord{...}/' will have a fancy typesetting
                        //               without a clamsy unexpected space filling.
                        g_svg_page.tab.ln_info[s].x = 0;
                    }
                }
            }

            // INFO(Rafael): Let's start taking into consideration that the current section is not about bends or
            //               release bends.

            bend_arrow_string = 6;
            g_svg_page.tab.sched_cr = 0;

            for (s = 0; s < 6; s++) {
                // INFO(Rafael): Vertically traversing each section note in the ascii TAB.
                if (tp->strings[s][offset] == '-' ||
                    (offset > 0 && isdigit(tp->strings[s][offset-1]) && isdigit(tp->strings[s][offset]))) {
                    if (tp->strings[s][offset] == '-' && g_svg_page.tab.last_symbol != kTlpSepBar) {
                        g_svg_page.tab.last_symbol = kTlpNoteSep;
                    }
                    // INFO(Rafael): If is just about a ordinary separator or a note done from 10th fret or higher.
                    //               Notes are always entired written, so a note higher than '9' should be skipped
                    //               at second time.
                    if (is_chord && tp->strings[s][offset] == '-' &&
                        (offset + 1) < tp->fretboard_sz           &&
                        isdigit(tp->strings[s][offset + 1])       &&
                        xstep == svgtypesetter_note_sep_xstep) {
                        // INFO(Rafael): Using xstep when we got a chord and notes lower than '10' will ensure a fancy
                        //               alignment to if this chord also mix notes higher than '9'.
                        xstep = svgtypesetter_chord_span_xstep;
                    }
                    continue;
                }
                if (tp->strings[s][offset] == 'b' && bend_arrow_string == 6) {
                    // INFO(Rafael): If the current TAB section is about a bend, let's discover at what string iteration
                    //               an arrow should be drawn in SVG stream.
                    bend_arrow_string = svgtypesetter_get_bend_arrow_string((const char **)tp->strings, offset);
                } else if (tp->strings[s][offset] == 'r' && bend_arrow_string == 6) {
                    // INFO(Rafael): If the current TAB section is about a release bend, let's discover at what string
                    // iteration an arrow should be drawn in SVG stream.
                    bend_arrow_string = svgtypesetter_get_release_bend_arrow_string((const char **)tp->strings, offset);
                }
                // INFO(Rafael): Setting all handy control pointers to the current processed string.
                g_svg_page.tab.carriage_x = &g_svg_page.tab.fbrd[s].x;
                g_svg_page.tab.carriage_y = &g_svg_page.tab.fbrd[s].y;
                g_svg_page.tab.curr_ln_info = &g_svg_page.tab.ln_info[s];
                g_svg_page.tab.sched_cr = ((offset + 1 < tp->fretboard_sz) ? (tp->strings[s][offset + 1] != '-') : 0);
                // INFO(Rafael): Finally doing the flush of the current ascii TAB section to the SVG TAB section.
                do_pinch_flush(xstep, &tp->strings[s][offset], s, bend_arrow_string, is_chord);
            }

            if (is_chord) {
                // INFO(Rafael): Since a chord have just been flushed into SVG stream, we need to
                //               set all ln_info[].x to the maximum non-zero ln_info[].x and the
                //               ln_info[].len follows the same idea. Those data into ln_info can
                //               be used during carriage x best-fit computations at the next flush
                //               iteration if the next symbol is not an ordinary note sep ('-').
                ln_info.x = 0;
                ln_info.len = 1;
                ln_info.do_cr_arg = 0;
                ln_info.do_carriage_return = NULL;
                for (s = 0; s < 6; s++) {
                    if (ln_info.x < g_svg_page.tab.ln_info[s].x) {
                        ln_info.x = g_svg_page.tab.ln_info[s].x;
                        ln_info.do_cr_arg = g_svg_page.tab.ln_info[s].do_cr_arg;
                        ln_info.do_carriage_return = g_svg_page.tab.ln_info[s].do_carriage_return;
                    }
                    if (ln_info.len < g_svg_page.tab.ln_info[s].len) {
                        ln_info.len = g_svg_page.tab.ln_info[s].len;
                    }
                }
                for (s = 0; s < 6; s++) {
                    g_svg_page.tab.ln_info[s] = ln_info;
                }
            }

            // INFO(Rafael): At this point the current ascii TAB section was processed already. We need to step the
            //               current carriage x according to the data written to the SVG stream. We known how much
            //               to step by calling the function xstep passing '1' as its dit. If xstep is NULL it indicates
            //               that the processed ascii TAB section was composed only by ordinary '-' separators, we still
            //               have to step the carriage x, but not much.


            if (xstep != NULL) {
                // INFO(Rafael): Going ahead horizontally.
                xstep(1);
                // INFO(Rafael): We also need to known how to go back, if necessary at the next iteration.
                last_xstep = xstep;
                xstep = NULL;
                svgtypesetter_refresh_fbrd_xy();

                g_svg_page.tab.last_non_null_x = *g_svg_page.tab.carriage_x;

                if (g_svg_page.tab.fbrd[5].y >= (g_svgtypesetter_page_height - (SVGTYPESETTER_TAB_Y_SPAN * 6)) &&
                    tp->next != NULL) {
                    // INFO(Rafael): The current page became full, we need a new one.
                    svgtypesetter_newpage();
                }

                // DEPRECATED(Rafael): Code chunk relocated.
                /*if (*g_svg_page.tab.carriage_x >= (SVGTYPESETTER_PAGE_WIDTH - SVGTYPESETTER_TAB_X_SPAN) &&
                    has_unflushed_data((const char **)tp->strings, offset + 1, tp->fretboard_sz)) {
                    // INFO(Rafael): The current tab diagram became full, we need a new empty one.

                    if (stech_end != NULL) {
                        // INFO(Rafael): If a new TAB diagram will be requested, we need to finish up drawing
                        //               the sustained techniques lines of the current full one.
                        stech_p = &stech_pts[0];
                        while (stech_p != stech_end) {
                            fprintf(g_svg_page.fp, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\""
                                               " style=\"stroke:rgb(0,0,0);stroke-width:1;opacity:0.5\""
                                               " stroke-dasharray=\"5,5\"/>\n", stech_p->x, *g_svg_page.tab.carriage_x,
                                                                                stech_p->y + 1, stech_p->y + 1);
                            stech_p++;
                        }
                    }
                    svgtypesetter_newtabdiagram(tp);
                    if (stech_end != NULL) {
                        // INFO(Rafael): If a new TAB diagram requested, we need to indicate above this new one
                        //               all sustained techniques indicated above the old one. In this case, only
                        //               lines will be drawn.
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
                }*/
            } else {
                // INFO(Rafael): The current ascii TAB section was only about a section with ordinary '-' separators.
                //               We will advance the carriage x less than with other steppers.
                last_xstep = svgtypesetter_user_note_span_xstep;
                svgtypesetter_user_note_span_xstep(1);
                svgtypesetter_refresh_fbrd_xy();
                if (*g_svg_page.tab.carriage_x >= (g_svgtypesetter_page_width - SVGTYPESETTER_TAB_X_SPAN) &&
                    has_unflushed_data((const char **)tp->strings, offset + 1, tp->fretboard_sz)) {
                    // INFO(Rafael): The current tab diagram became full, we need a new empty one.
                    g_svg_page.tab.last_non_null_x = *g_svg_page.tab.carriage_x;
                }
            }

            if (*g_svg_page.tab.carriage_x >= (g_svgtypesetter_page_width - SVGTYPESETTER_TAB_X_SPAN) &&
                has_unflushed_data((const char **)tp->strings, offset + 1, tp->fretboard_sz)) {
                // INFO(Rafael): The current tab diagram became full, we need a new empty one.

                if (stech_end != NULL) {
                    // INFO(Rafael): If a new TAB diagram will be requested, we need to finish up drawing
                    //               the sustained techniques lines of the current full one.
                    stech_p = &stech_pts[0];
                    while (stech_p != stech_end) {
                        fprintf(g_svg_page.fp, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\""
                                           " style=\"stroke:rgb(0,0,0);stroke-width:1;opacity:0.5\""
                                           " stroke-dasharray=\"5,5\"/>\n", stech_p->x, *g_svg_page.tab.carriage_x,
                                                                            stech_p->y + 1, stech_p->y + 1);
                        stech_p++;
                    }
                }
                svgtypesetter_newtabdiagram(tp);
                if (stech_end != NULL) {
                    // INFO(Rafael): If a new TAB diagram requested, we need to indicate above this new one
                    //               all sustained techniques indicated above the old one. In this case, only
                    //               lines will be drawn.
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

            if (times != NULL) {
                // INFO(Rafael): The buffer pointer must be incremented because it was written above the entire ascii TAB
                //               diagram.
                times++;
            }
        }

        // INFO(Rafael): All ascii TAB was flushed into the SVG TAB. We only need to refresh some horizontal coordinates for
        //               a fancy typesetting and call svgtypesetter_cut_tab() to ensure that no empty TAB space will be let
        //               on the last drawn TAB diagram.

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
