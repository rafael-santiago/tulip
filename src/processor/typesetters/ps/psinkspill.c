/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/ps/psinkspill.h>
#include <processor/typesetters/ps/psboundaries.h>
#include <processor/typesetters/typeprefs.h>
#include <processor/oututils.h>
#include <processor/settings.h>
#include <system/version.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int g_ps_cpage = 0;

struct pstypesetter_real_x_coords_ctx {
    int value;
};

struct pstypesetter_tab_diagram_ctx {
    int xlimit_left, xlimit_right;
    int carriage_y, carriage_x;
    int start_offset, end_offset;
    int sustained_techniques_y;
    struct pstypesetter_real_x_coords_ctx x[PSTYPESETTER_FRETBOARD_SIZE];
};

static int g_ps_pagenumbering = 1;

static struct pstypesetter_tab_diagram_ctx g_ps_ctab;

static void pstypesetter_init(void);

static FILE *pstypesetter_newps(const char *filepath);

static void pstypesetter_closeps(FILE *fp);

static void pstypesetter_newpage(FILE *fp);

static void pstypesetter_showpage(FILE *fp);

static void pstypesetter_newtabdiagram(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void pstypesetter_proc_tabchunk(FILE *fp, const txttypesetter_tablature_ctx *tab);

static int pstypesetter_string_y(const int sn, const int cy);

static int pstypesetter_pinch_y(const int sn, const int cy);

static void pstypesetter_flush_fretboard_pinches(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void pstypesetter_spill_sustained_techniques(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void pstypesetter_eval_sustained_techniques_area(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void pstypesetter_spill_times(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void pstypesetter_spill_comments(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void pstypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song);

static void pstypesetter_spill_transcribers_name(FILE *fp, const char *transcriber);

static void pstypesetter_spill_song_title(FILE *fp, const char *song);

static void pstypesetter_spill_fretboard_tunning(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void pstypesetter_vertbar(FILE *fp, const int x, const int y, const int sn);

static void pstypesetter_pinch_hammer_on_pull_off(FILE *fp, const int x, const int y);

static void pstypesetter_pinch_vibrato(FILE *fp, const int x, const int y);

static void pstypesetter_pinch_bend(FILE *fp, const int x, const int y, const int pointed);

static void pstypesetter_pinch_release_bend(FILE *fp, const int x, const int y, const int pointed);

static void pstypesetter_spill_tunning(FILE *fp);

static void pstypesetter_init(void) {
    g_ps_cpage = 0;
}

static void pstypesetter_pageinit(void) {
    g_ps_ctab.xlimit_left = PSTYPESETTER_PAGEXL;
    g_ps_ctab.xlimit_right = PSTYPESETTER_PAGEXR;
    g_ps_ctab.carriage_y = PSTYPESETTER_PAGEY;
    g_ps_ctab.sustained_techniques_y = PSTYPESETTER_PAGEY;
}

static FILE *pstypesetter_newps(const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (fp == NULL) {
        return NULL;
    }
    fprintf(fp, "%%!PS-Adobe-3.0\n"
                "%%Generated with tulip-%s\n"
                "/Times-Bold 11 selectfont\n", get_tulip_system_version());
    return fp;
}

static void pstypesetter_closeps(FILE *fp) {
    pstypesetter_showpage(fp);
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
    char pn[255] = "";

    // WARN(Rafael): The right way of centering it is to load the font width from the acrobat font info file and so
    //               do all the math based on this besides the current font size... however the things here is kind
    //               of "static" then this heuristic works pretty well.
    if (g_ps_pagenumbering) {
        sprintf(pn, "%d", g_ps_cpage);
        fprintf(fp, "/Times-Italic 11 selectfont\n"
                    "%d %d moveto (%s) show\n"
                    "/Times-Bold 11 selectfont\n", (PSTYPESETTER_PAGEXR / 2) - strlen(pn) - 1,
                                                    PSTYPESETTER_PAGEY_LIMIT + 20, pn);
    }

    fprintf(fp, "showpage\n");
}

static void pstypesetter_newtabdiagram(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    int s = 0;
    int sy = 0;
    struct typesetter_curr_settings cset = typesetter_settings();

    if (is_tab_empty(tab, -1)) {
        return;
    }

    for (s = 0; s < tab->string_nr; s++) {
        if (pstypesetter_string_y(s, g_ps_ctab.carriage_y) < PSTYPESETTER_PAGEY_LIMIT + 20) {
            pstypesetter_pageinit();
            pstypesetter_showpage(fp);
            pstypesetter_newpage(fp);
            pstypesetter_eval_sustained_techniques_area(fp, tab);
            break;
        }
    }

    fprintf(fp, "%.1f setlinewidth\n", PSTYPESETTER_TABLINE_W);

    g_ps_ctab.carriage_x = PSTYPESETTER_CARRIAGEX;

    for (s = 0; s < tab->string_nr; s++) {
        sy = pstypesetter_string_y(s, g_ps_ctab.carriage_y);
        fprintf(fp, "%d %d moveto\n"
                    "%d %d lineto\n", g_ps_ctab.xlimit_left, sy, g_ps_ctab.xlimit_right, sy);
    }

    fprintf(fp, "stroke\n");

    fprintf(fp, "/Times-Bold 13 selectfont\n");

    fprintf(fp, "%d %d moveto (T) show\n", PSTYPESETTER_PAGEXL + 2, pstypesetter_pinch_y(1, g_ps_ctab.carriage_y) + 1);
    fprintf(fp, "%d %d moveto (A) show\n", PSTYPESETTER_PAGEXL + 2, pstypesetter_pinch_y(3, g_ps_ctab.carriage_y) + 2);
    fprintf(fp, "%d %d moveto (B) show\n", PSTYPESETTER_PAGEXL + 2, pstypesetter_pinch_y(5, g_ps_ctab.carriage_y) + 4);

    fprintf(fp, "/Times-Bold 11 selectfont\n");

    if (cset.prefs & kTlpPrefsAddTunningToTheFretboard) {
        pstypesetter_spill_fretboard_tunning(fp, tab);
    } else if (cset.prefs & kTlpPrefsFretboardStyleNormal) {
        pstypesetter_vertbar(fp, g_ps_ctab.xlimit_left, g_ps_ctab.carriage_y, tab->string_nr);
    }

    for (s = 0; s < PSTYPESETTER_FRETBOARD_SIZE; s++) {
        g_ps_ctab.x[s].value = g_ps_ctab.xlimit_right;
    }
}

static void pstypesetter_spill_fretboard_tunning(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    size_t s;

    for (s = 0; s < tab->string_nr; s++) {
        fprintf(fp, "%d %d moveto (%s) show\n", g_ps_ctab.xlimit_left - PSTYPESETTER_CARRIAGE_STEP - 10,
                                                pstypesetter_pinch_y(s, g_ps_ctab.carriage_y) + 2, tab->tunning[s]);
    }
}

static int pstypesetter_string_y(const int sn, const int cy) {
    return (cy + (PSTYPESETTER_NEXT_STRING * sn));
}

static int pstypesetter_pinch_y(const int sn, const int cy) {
    return (pstypesetter_string_y(sn + 1, cy) - PSTYPESETTER_ONTO_STRING_DELTA);
}

static void pstypesetter_proc_tabchunk(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    if (g_ps_cpage == 0) {
        pstypesetter_newpage(fp);
    }

    pstypesetter_spill_comments(fp, tab);
    pstypesetter_eval_sustained_techniques_area(fp, tab);
    pstypesetter_spill_times(fp, tab);

    pstypesetter_newtabdiagram(fp, tab);

    pstypesetter_flush_fretboard_pinches(fp, tab);
}

static void pstypesetter_vertbar(FILE *fp, const int x, const int y, const int sn) {
    fprintf(fp, "%d %d moveto\n"
                "%d %d lineto\n"
                "stroke\n", x, pstypesetter_string_y(0, y), x, pstypesetter_string_y(sn - 1, y));
}

static void pstypesetter_pinch_hammer_on_pull_off(FILE *fp, const int x, const int y) {
    fprintf(fp, "/Times-Bold 20 selectfont\n");
    fprintf(fp, "gsave %d %d moveto 90 rotate (\\)) show grestore\n", x, y);
    fprintf(fp, "/Times-Bold 11 selectfont\n");
}

static void pstypesetter_pinch_vibrato(FILE *fp, const int x, const int y) {
    fprintf(fp, "/Times-Bold 20 selectfont\n"
                "%d %d moveto (\\~) show\n"
                "/Times-Bold 11 selectfont\n", x, y);
}

static void pstypesetter_pinch_bend(FILE *fp, const int x, const int y, const int pointed) {
    fprintf(fp, "/Times-Italic 25 selectfont\n"
                "gsave %d %d moveto %d rotate (\\)) show grestore\n", x, y, (pointed) ? -5 : -7);

    if (pointed) {
        fprintf(fp, "newpath\n"
                    "%d.5 %d moveto\n"
                    "%d.5 %d lineto\n"
                    "%d.5 %d lineto\n"
                    "closepath\n"
                    "fill\n", x + 3, y + 14, x + 6, y + 19, x + 9, y + 14);
    }

    fprintf(fp, "/Times-Bold 11 selectfont\n");
}

static void pstypesetter_pinch_release_bend(FILE *fp, const int x, const int y, const int pointed) {
    fprintf(fp, "/Times-Italic 25 selectfont\n"
                "gsave %d %d moveto -150 rotate (\\() show grestore\n", x + 6, y);

    if (pointed) {
        fprintf(fp, "newpath\n"
                    "%d.5 %d moveto\n"
                    "%d.5 %d lineto\n"
                    "%d.5 %d lineto\n"
                    "closepath\n"
                    "fill\n", x + 4, y - 15, x + 7, y - 20, x + 10, y - 15);
    }

    fprintf(fp, "/Times-Bold 11 selectfont\n");
}

static void pstypesetter_close_tab(FILE *fp, const int x, const int sn) {
    int yt = 0, yb = 0;

    if (x >= g_ps_ctab.xlimit_right) {
        return;
    }

    /* WARN(Rafael): !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                     -------- HANNA-BARBERA's DASTARDLY DICK WAS HERE --------
                     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! >:) Hahaha!!
    */

    yt = g_ps_ctab.sustained_techniques_y + 10;
    yb = pstypesetter_string_y(sn - 1, g_ps_ctab.carriage_y) - 10;

    fprintf(fp, "1 1 1 setrgbcolor\n"
                "newpath\n"
                "%d %d moveto\n"
                "%d %d lineto\n"
                "%d %d lineto\n"
                "closepath\n"
                "fill\n"
                "newpath\n"
                "%d %d moveto\n"
                "%d %d lineto\n"
                "%d %d lineto\n"
                "closepath\n"
                "fill\n"
                "0 0 0 setrgbcolor\n", x, yt,
                                       x, yb,
                                       g_ps_ctab.xlimit_right, yt,
                                       g_ps_ctab.xlimit_right, yb,
                                       g_ps_ctab.xlimit_right, yt,
                                       x, yb);

    pstypesetter_vertbar(fp, x, g_ps_ctab.carriage_y, sn);
}

static void pstypesetter_flush_fretboard_pinches(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    size_t s, o;
    int x = g_ps_ctab.carriage_x;
    int pointed = 0;
    struct typesetter_curr_settings cset = typesetter_settings();
    size_t s_limit = tab->fretboard_sz;

    if (is_tab_empty(tab, -1)) {
        return;
    }

    if (cset.prefs & kTlpPrefsCutTabOnTheLastNote) {
        s_limit = get_fretboard_usage_limit(tab);
    }

    g_ps_ctab.start_offset = 0;

    for (o = 0; o < s_limit; o++) {

        g_ps_ctab.end_offset = o;

        for (s = 0; s < tab->string_nr; s++) {

            if (s == 0) {
                g_ps_ctab.x[o].value = x;
            }

            switch (tab->strings[s][o]) {
                case 'h':
                case 'p':
                    x += PSTYPESETTER_CARRIAGE_STEP + 10;

                    if (x >= g_ps_ctab.xlimit_right && !is_tab_empty(tab, o)) {
                        pstypesetter_spill_sustained_techniques(fp, tab);
                        g_ps_ctab.start_offset = o;

                        if ((cset.prefs & kTlpPrefsFretboardStyleNormal    ) ||
                            (cset.prefs & kTlpPrefsCloseTabToSave          ) ||
                            ((cset.prefs & kTlpPrefsFretboardStyleContinuous) && tab->next == NULL)) {
                                pstypesetter_close_tab(fp, x, tab->string_nr);
                        }
                        g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_TABCHUNK + PSTYPESETTER_NEXT_ADDINFO;
                        pstypesetter_eval_sustained_techniques_area(fp, tab);
                        pstypesetter_newtabdiagram(fp, tab);
                        x = g_ps_ctab.carriage_x + PSTYPESETTER_CARRIAGE_STEP + 10;
                    }

                    pstypesetter_pinch_hammer_on_pull_off(fp, x, pstypesetter_pinch_y(s, g_ps_ctab.carriage_y) + 5);

                    if (s < tab->string_nr - 1 && (tab->strings[s+1][o] == 'h' || tab->strings[s+1][o] == 'p')) {
                        x -= PSTYPESETTER_CARRIAGE_STEP + 10;
                    }
                    break;

                case '~':
                    pstypesetter_pinch_vibrato(fp, x, pstypesetter_pinch_y(s, g_ps_ctab.carriage_y) + 3);
                    if (s == tab->string_nr - 1 || tab->strings[s+1][o] != '~') {
                        x += PSTYPESETTER_CARRIAGE_STEP;
                    }
                    break;

                case 'b':
                    pointed = (s == 0 || tab->strings[s - 1][o] != 'b');
                    pstypesetter_pinch_bend(fp, x, pstypesetter_pinch_y(s, g_ps_ctab.carriage_y) + 5, pointed);
                    break;

                case 'r':
                    pointed = (s == tab->string_nr - 1 || tab->strings[s + 1][o] != 'r');
                    pstypesetter_pinch_release_bend(fp, x, pstypesetter_pinch_y(s, g_ps_ctab.carriage_y), pointed);
                    break;

                case '-':
                    break;

                case '|':
                    pstypesetter_vertbar(fp, x, g_ps_ctab.carriage_y, tab->string_nr);
                    s = tab->string_nr + 1;
                    x -= PSTYPESETTER_CARRIAGE_STEP;
                    continue;

                default:
                    if (tab->strings[s][o] == '/' || tab->strings[s][o] == '\\') {
                        x += 1;

                        if (x >= g_ps_ctab.xlimit_right && !is_tab_empty(tab, o)) {
                            pstypesetter_spill_sustained_techniques(fp, tab);
                            g_ps_ctab.start_offset = o;

                            if ((cset.prefs & kTlpPrefsFretboardStyleNormal    ) ||
                                (cset.prefs & kTlpPrefsCloseTabToSave          ) ||
                                ((cset.prefs & kTlpPrefsFretboardStyleContinuous) && tab->next == NULL)) {
                                    pstypesetter_close_tab(fp, x, tab->string_nr);
                            }
                            g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_TABCHUNK + PSTYPESETTER_NEXT_ADDINFO;
                            pstypesetter_eval_sustained_techniques_area(fp, tab);
                            pstypesetter_newtabdiagram(fp, tab);
                            x = g_ps_ctab.carriage_x;
                        }
                    }

                    fprintf(fp, "%d %d moveto (%s%c) show\n", x, pstypesetter_pinch_y(s, g_ps_ctab.carriage_y),
                                                            (tab->strings[s][o] == '\\') ? "\\" : "",
                                                                tab->strings[s][o]);

                    if (tab->strings[s][o] == '/' || tab->strings[s][o] == '\\') {
                        x -= 1;
                    }
                    break;
            }
        }

        x += PSTYPESETTER_CARRIAGE_STEP;

        /*if (tab->techniques != NULL) {
            if (o > 0 && tab->techniques->data != NULL &&
                tab->techniques->data[o] != '.' && tab->techniques->data[o-1] == '.') {
                x += PSTYPESETTER_CARRIAGE_STEP;
            }
        }*/

        if (x >= g_ps_ctab.xlimit_right && !is_tab_empty(tab, o)) {
            pstypesetter_spill_sustained_techniques(fp, tab);
            g_ps_ctab.start_offset = o;

            if ((cset.prefs & kTlpPrefsFretboardStyleNormal    ) ||
                (cset.prefs & kTlpPrefsCloseTabToSave          ) ||
                ((cset.prefs & kTlpPrefsFretboardStyleContinuous) && tab->next == NULL)) {
                pstypesetter_close_tab(fp, x, tab->string_nr);
            }

            g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_TABCHUNK + PSTYPESETTER_NEXT_ADDINFO;

            pstypesetter_eval_sustained_techniques_area(fp, tab);
            pstypesetter_newtabdiagram(fp, tab);
            x = g_ps_ctab.carriage_x;
        }
    }

    pstypesetter_spill_sustained_techniques(fp, tab);

    if ((cset.prefs & kTlpPrefsFretboardStyleNormal    ) ||
        (cset.prefs & kTlpPrefsCloseTabToSave          ) ||
        ((cset.prefs & kTlpPrefsFretboardStyleContinuous) && tab->next == NULL && !is_tab_empty(tab, o))) {
        pstypesetter_close_tab(fp, x, tab->string_nr);
    }

    g_ps_ctab.carriage_x = PSTYPESETTER_CARRIAGEX;

    g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_TABCHUNK;
}

static void pstypesetter_spill_song_title(FILE *fp, const char *song) {
    if (song == NULL) {
        return;
    }
    fprintf(fp, "/Times-Bold 20 selectfont\n");
    fprintf(fp, "%d %d moveto (%s) show\n", PSTYPESETTER_PAGEXL, g_ps_ctab.carriage_y, song);
    fprintf(fp, "/Times-Bold 11 selectfont\n");
    g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO;
}

static void pstypesetter_spill_transcribers_name(FILE *fp, const char *transcriber) {
    if (transcriber == NULL) {
        return;
    }
    fprintf(fp, "/Times-Bold-Italic 11 selectfont\n");
    fprintf(fp, "%d %d moveto (transcribed by %s) show\n", PSTYPESETTER_PAGEXL, g_ps_ctab.carriage_y, transcriber);
    fprintf(fp, "/Times-Bold 11 selectfont\n");
    g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO * 3;
}

static void pstypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song) {
    struct typesetter_curr_settings cset = typesetter_settings();
    tulip_command_t used_techniques[31];
    size_t used_techniques_nr = 0, u = 0;
    int has_muffled = 0, has_anyfret = 0;
    int hp_done = 0;

    if ((cset.prefs & kTlpPrefsIncludeTabNotation) == 0 || song == NULL) {
        return;
    }

    g_ps_ctab.carriage_y -= PSTYPESETTER_NEXT_ADDINFO;

    get_all_used_techniques(song, used_techniques, &used_techniques_nr, &has_muffled, &has_anyfret);

    if (used_techniques_nr > 0 || has_muffled || has_anyfret) {
        for (u = 0; u < used_techniques_nr; u++) {
            switch (used_techniques[u]) {
                case kTlpHammerOn:
                case kTlpPullOff:
                    if (!hp_done) {
                        hp_done = 1;
                        pstypesetter_pinch_hammer_on_pull_off(fp, PSTYPESETTER_CARRIAGEX + 20, g_ps_ctab.carriage_y);
                        fprintf(fp, "%d %d moveto (=   %s/%s) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25,
                                                                     g_ps_ctab.carriage_y,
                                                                     get_technique_notation_label(kTlpHammerOn),
                                                                     get_technique_notation_label(kTlpPullOff));
                        g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO;
                    }
                    continue;

                case kTlpVibrato:
                    pstypesetter_pinch_vibrato(fp, PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.carriage_y);
                    fprintf(fp, "%d %d moveto (=   %s) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25,
                                                                g_ps_ctab.carriage_y,
                                                                get_technique_notation_label(kTlpVibrato));
                    g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO;
                    continue;

                case kTlpBend:
                    g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO;
                    pstypesetter_pinch_bend(fp, PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.carriage_y, 1);
                    fprintf(fp, "%d %d moveto (=   %s) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25,
                                                                g_ps_ctab.carriage_y,
                                                                get_technique_notation_label(kTlpBend));
                    g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO;
                    continue;

                case kTlpReleaseBend:
                    pstypesetter_pinch_release_bend(fp, PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.carriage_y, 1);
                    fprintf(fp, "%d %d moveto (=   %s) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25,
                                                                g_ps_ctab.carriage_y - 10,
                                                                get_technique_notation_label(kTlpReleaseBend));
                    g_ps_ctab.carriage_y += (2 * PSTYPESETTER_NEXT_ADDINFO);
                    continue;

                case kTlpSlideUp:
                        fprintf(fp, "%d %d moveto (\\\\) show\n", PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.carriage_y);
                    break;

                default:
                    fprintf(fp, "%d %d moveto (%s) show\n", PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.carriage_y,
                                                            get_technique_label(used_techniques[u]));
                    break;
            }

            fprintf(fp, "%d %d moveto (=   %s) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25,
                                                        g_ps_ctab.carriage_y,
                                                        get_technique_notation_label(used_techniques[u]));


            g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO;
        }

        if (has_muffled) {
            fprintf(fp, "%d %d moveto (X) show\n", PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.carriage_y);
            fprintf(fp, "%d %d moveto (=   Muffled note) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25,
                                                                  g_ps_ctab.carriage_y);
            g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO;
        }

        if (has_anyfret) {
            fprintf(fp, "%d %d moveto (?) show\n", PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.carriage_y);
            fprintf(fp, "%d %d moveto (=   From any fret) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25,
                                                                   g_ps_ctab.carriage_y);
            g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO;
        }
    }

    g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO;

}

static void pstypesetter_spill_tunning(FILE *fp) {
    int t = 0;
    size_t t_nr = 0;
    char **tunning = NULL;
    struct typesetter_curr_settings cset = typesetter_settings();

    if ((cset.prefs & kTlpPrefsShowTunning) && !(cset.prefs & kTlpPrefsAddTunningToTheFretboard)) {

        if (!(cset.prefs & kTlpPrefsIncludeTabNotation)) {
            g_ps_ctab.carriage_y -= PSTYPESETTER_NEXT_ADDINFO;
        }

        tunning = get_processor_setting("tunning", &t_nr);

        fprintf(fp, "%d %d moveto (Tunning [%d-1] = ", PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.carriage_y, t_nr);

        for (t = t_nr - 1; t >= 0; t--) {
            fprintf(fp, "%s", tunning[t]);
            if (t != 0) {
                fprintf(fp, ", ");
            }
        }

        fprintf(fp, ") show\n");

        g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO;

    }

    g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO;
}

static void pstypesetter_spill_comments(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    const txttypesetter_comment_ctx *cp = NULL;
    char *dp = NULL, *dp_end = NULL;
    char comment[65535] = "";
    size_t c = 0;

    if (tab == NULL || tab->comments == NULL) {
        return;
    }

    if (tab->last != NULL) {
        g_ps_ctab.carriage_y += (2 * PSTYPESETTER_NEXT_ADDINFO);
    }

    if (g_ps_ctab.carriage_y < (PSTYPESETTER_PAGEY_LIMIT + 20)) {
        pstypesetter_pageinit();
        pstypesetter_showpage(fp);
        pstypesetter_newpage(fp);
    }

    fprintf(fp, "/Times-Italic 11 selectfont\n");

    for (cp = tab->comments; cp != NULL; cp = cp->next) {
        dp = cp->data;
        dp_end = dp + strlen(dp);
        while (dp < dp_end) {
            c = 0;
            while (dp != dp_end && *dp != '\n') {
                comment[c] = *dp;
                c = (c + 1) % sizeof(comment);
                dp++;
            }
            comment[c] = 0;
            fprintf(fp, "%d %d moveto (%s) show\n", PSTYPESETTER_PAGEXL, g_ps_ctab.carriage_y, comment);
            g_ps_ctab.carriage_y += PSTYPESETTER_ADDINFO_LINEBREAK;
            dp++;
        }
    }

    fprintf(fp, "/Times-Bold 11 selectfont\n");

    g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO;
}

static void pstypesetter_spill_times(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    const char *tp = tab->times;
    const char *tp_end = NULL;
    int x = PSTYPESETTER_CARRIAGEX;
    size_t s = 0;
    int has_half_step_notes = tunning_has_half_step_notes(tab, NULL, typesetter_settings().prefs);
    int print_times = 0;

    if (tp == NULL) {
        return;
    }

    tp_end = tp + strlen(tp);

    while (tp != tp_end && !print_times) {
        print_times = isdigit(*tp);
        tp++;
    }

    if (!print_times) {
        return;
    }

    tp = tab->times;

    if (has_half_step_notes) {
        x += PSTYPESETTER_CARRIAGE_STEP;
    }

    while (tp != tp_end) {

        if (*tp != ' ') {
            fprintf(fp, "%d %d moveto (%c) show\n", x, g_ps_ctab.carriage_y, *tp);
        }

        for (s = 0; s < tab->string_nr; s++) {
            switch (tab->strings[s][tp_end - tp]) {
                case 'h':
                case 'p':
                    x += PSTYPESETTER_CARRIAGE_STEP + 10;
                    if (s < tab->string_nr - 1 && (tab->strings[s+1][tp_end - tp] == 'h' ||
                                                   tab->strings[s+1][tp_end - tp] == 'p')) {
                        x -= PSTYPESETTER_CARRIAGE_STEP + 10;
                    }
                    break;

                case '~':
                    if (s == tab->string_nr - 1 || tab->strings[s+1][tp_end - tp] != '~') {
                        x += PSTYPESETTER_CARRIAGE_STEP;
                    }
                    break;
            }
        }

        for (s = 0; s < tab->string_nr; s++) {
            if (tab->strings[s][tp_end - tp] == '|') {
                x -= PSTYPESETTER_CARRIAGE_STEP;
                break;
            }
        }

        x += PSTYPESETTER_CARRIAGE_STEP;

        tp++;
    }

    g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_STRING;
}

static void pstypesetter_spill_sustained_techniques(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    int cy = g_ps_ctab.sustained_techniques_y, xl, xr = 0;
    const txttypesetter_sustained_technique_ctx *tp = NULL;
    char label[255] = "", *dp = NULL;
    size_t l = 0, d = 0, d_end = 0;

    /* ------------------------------------------------------------------------------------------------------------------------
     * --------------------------------- TIP(Monty Python's Black Knight): You Shall Not Pass. --------------------------------
     * ------------------------------------------------------------------------------------------------------------------------
     *
     * WARN(Rafael): The sustained techniques are "spit out" in... "2 acts". It is a damn craziness I must confess but works.
     *
     *  The fisrt function [pstypesetter_eval_sustained_techniques_area()] only stores the initial y of the first technique
     *  and let a correct space to be filled later. The [pstypesetter_eval_sustained_techniques_area()] function just
     *  "make the plans about" :D and like an ostrich calls abstract(). I really should have called it professor_moriarty().
     *
     *  So, the second act of the Crime is performed by this current function. Basically, this function restores the current
     *  tab's carriage_y to the initial y of the first sustained technique (a.k.a. Crime scene) and traverses the related
     *  linked list writing (flushing) all data...
     *
     *  However, the tablature diagram can be not enough as its reference ASCII representation (in RAM). Frequently what the
     *  Tulip has into RAM is not an absolute truth when in Postscript. Additonal tab diagrams can be necessary when our
     *  moriarty-function has gone away.
     *
     *  The solution here is to flush the data following a kind of sliding window strategy, incantations and black art.
     *
     *  Also important: this current function is triggered only by [pstypesetter_flush_fretboard_pinches()].
     *
     *  Well, good luck! ;)
     *
     * ------------------------------------------------------------------------------------------------------------------------
     */

    fprintf(fp, "gsave [3 3] 0 setdash\n");

    for (tp = tab->techniques; tp != NULL; tp = tp->next) {
        d = g_ps_ctab.start_offset;

        while (d > 0 && isalpha(*(tp->data + d - 1))) {
            d--;
        }

        d_end = g_ps_ctab.end_offset + 1;

        while (d < d_end) {

            dp = tp->data + d;

            if (*dp != ' ') {

                if (isalpha(*dp)) {

                    memset(label, 0, sizeof(label));

                    l = 0;
                    while (d < d_end && isalpha(*dp)) {
                        label[l] = *dp;
                        l = (l + 1) % sizeof(label);
                        d++;
                        dp = tp->data + d;
                    }

                    if (g_ps_ctab.x[d].value >= g_ps_ctab.xlimit_right) {
                        continue;
                    }

                    fprintf(fp, "%d %d moveto (%s) show\n", g_ps_ctab.x[d].value, cy, label);

                    if (d_end < PSTYPESETTER_FRETBOARD_SIZE) {
                        d_end++;
                    }

                } else if (*dp == '.') {

                    xl = g_ps_ctab.x[d].value;

                    while (d < d_end && (*dp == '.' || (*dp + 1) == '.')) {
                        d++;
                        dp = tp->data + d;
                    }

                    if (d < d_end) {
                        xr = g_ps_ctab.x[d].value;
                    } else {
                        xr = g_ps_ctab.x[d - 1].value;
                    }

                    if (xr > g_ps_ctab.xlimit_right) {
                        xr = g_ps_ctab.xlimit_right;
                    }

                    if (xl >= g_ps_ctab.xlimit_right) {
                        xl = PSTYPESETTER_CARRIAGEX;
                    }

                    fprintf(fp, "%d %d moveto %d %d lineto stroke\n", xl, cy, xr, cy);
                }
            }

            d++;
        }

        cy += PSTYPESETTER_ADDINFO_LINEBREAK;
    }

    fprintf(fp, "grestore\n");
}

static void pstypesetter_eval_sustained_techniques_area(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    const txttypesetter_sustained_technique_ctx *tp = NULL;

    if (is_tab_empty(tab, -1)) {
        return;
    }

    if (tab->comments == NULL) {
        g_ps_ctab.carriage_y += (2 * PSTYPESETTER_NEXT_ADDINFO);
    }

    if (g_ps_ctab.carriage_y < (PSTYPESETTER_PAGEY_LIMIT + 60)) {
        pstypesetter_pageinit();
        pstypesetter_showpage(fp);
        pstypesetter_newpage(fp);
    }
    g_ps_ctab.sustained_techniques_y = g_ps_ctab.carriage_y;

    for (tp = tab->techniques; tp != NULL; tp = tp->next) {
        g_ps_ctab.carriage_y += PSTYPESETTER_ADDINFO_LINEBREAK;
    }

    g_ps_ctab.carriage_y += PSTYPESETTER_NEXT_ADDINFO;
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
    pstypesetter_newpage(fp);

    pstypesetter_spill_song_title(fp, tab->song);
    pstypesetter_spill_transcribers_name(fp, tab->transcriber);

    pstypesetter_spill_tab_notation(fp, song);

    pstypesetter_spill_tunning(fp);

    for (tp = tab; tp != NULL; tp = tp->next) {
        pstypesetter_proc_tabchunk(fp, tp);
    }

    pstypesetter_closeps(fp);

    return 1;
}

void pstypesetter_disable_pagenumbering(void) {
    g_ps_pagenumbering = 0;
}

void pstypesetter_enable_pagenumbering(void) {
    g_ps_pagenumbering = 1;
}

int pstypesetter_get_carriage_y(void) {
    return g_ps_ctab.carriage_y;
}
