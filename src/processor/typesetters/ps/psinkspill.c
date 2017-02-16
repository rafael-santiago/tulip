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

static int g_ps_ctab_sustained_techniques_y = 0;

struct pstypesetter_tab_diagram_ctx {
    int cxl, cxr;
    int cy, cx;
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

    if (is_tab_empty(tab)) {
        return;
    }

    for (s = 0; s < tab->string_nr; s++) {
        if (pstypesetter_string_y(s, g_ps_ctab.cy) < PSTYPESETTER_PAGEY_LIMIT) {
            pstypesetter_pageinit();
            pstypesetter_showpage(fp);
            pstypesetter_newpage(fp);
            break;
        }
    }


    fprintf(fp, "%.1f setlinewidth\n", PSTYPESETTER_TABLINE_W);

    g_ps_ctab.cx = PSTYPESETTER_CARRIAGEX;

    for (s = 0; s < tab->string_nr; s++) {
        sy = pstypesetter_string_y(s, g_ps_ctab.cy);
        fprintf(fp, "%d %d moveto\n"
                    "%d %d lineto\n", g_ps_ctab.cxl, sy, g_ps_ctab.cxr, sy);
    }

    fprintf(fp, "stroke\n");

    fprintf(fp, "/Times-Bold 13 selectfont\n");

    fprintf(fp, "%d %d moveto (T) show\n", PSTYPESETTER_PAGEXL + 2, pstypesetter_pinch_y(1, g_ps_ctab.cy) + 1);
    fprintf(fp, "%d %d moveto (A) show\n", PSTYPESETTER_PAGEXL + 2, pstypesetter_pinch_y(3, g_ps_ctab.cy) + 2);
    fprintf(fp, "%d %d moveto (B) show\n", PSTYPESETTER_PAGEXL + 2, pstypesetter_pinch_y(5, g_ps_ctab.cy) + 4);

    fprintf(fp, "/Times-Bold 11 selectfont\n");

    if (cset.prefs & kTlpPrefsAddTunningToTheFretboard) {
        pstypesetter_spill_fretboard_tunning(fp, tab);
    } else if (cset.prefs & kTlpPrefsFretboardStyleNormal) {
        pstypesetter_vertbar(fp, g_ps_ctab.cxl, g_ps_ctab.cy, tab->string_nr);
    }
}

static void pstypesetter_spill_fretboard_tunning(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    size_t s;

    for (s = 0; s < tab->string_nr; s++) {
        fprintf(fp, "%d %d moveto (%s) show\n", g_ps_ctab.cxl - PSTYPESETTER_CARRIAGE_STEP - 10,
                                                pstypesetter_pinch_y(s, g_ps_ctab.cy) + 2, tab->tunning[s]);
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
    pstypesetter_spill_sustained_techniques(fp, tab);
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
    int s = 0, sy = 0;
    int yt = 0, yb = 0;

    /* WARN(Rafael): !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                     -------- HANNA-BARBERA's DASTARDLY DICK WAS HERE --------
                     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! >:) Hahaha!!
    */

    yt = g_ps_ctab_sustained_techniques_y + 10;
    yb = pstypesetter_string_y(sn - 1, g_ps_ctab.cy) - 10;

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
                "newpath\n"
                "%d.5 %d.5 moveto\n"
                "%d.5 %d.5 lineto\n"
                "%d.5 %d.5 lineto\n"
                "closepath\n"
                "fill\n"
                "0 0 0 setrgbcolor\n", x, yt,
                                       x, yb,
                                       g_ps_ctab.cxr, yt,
                                       g_ps_ctab.cxr, yb,
                                       g_ps_ctab.cxr, yt,
                                       x, yb,
                                       x - 2, yt,
                                       x - 2, pstypesetter_string_y(0, g_ps_ctab.cy) + 10,
                                       g_ps_ctab.cxr, yt);

    pstypesetter_vertbar(fp, x, g_ps_ctab.cy, sn);
}

static void pstypesetter_flush_fretboard_pinches(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    size_t s, o;
    int x = g_ps_ctab.cx, py = 0;
    int pointed = 0;
    struct typesetter_curr_settings cset = typesetter_settings();
    size_t s_limit = tab->fretboard_sz;

    if (is_tab_empty(tab)) {
        return;
    }

    if (cset.prefs & kTlpPrefsCutTabOnTheLastNote) {
        s_limit = get_fretboard_usage_limit(tab);
    }

    for (o = 0; o < s_limit; o++) {

        for (s = 0; s < tab->string_nr; s++) {

            switch (tab->strings[s][o]) {
                case 'h':
                case 'p':
                    x += PSTYPESETTER_CARRIAGE_STEP + 10;

                    if (x >= g_ps_ctab.cxr) {
                        if ((cset.prefs & kTlpPrefsFretboardStyleNormal    ) ||
                            (cset.prefs & kTlpPrefsCloseTabToSave          ) ||
                            ((cset.prefs & kTlpPrefsFretboardStyleContinuous) && tab->next == NULL)) {
                                pstypesetter_close_tab(fp, x, tab->string_nr);
                        }

                        pstypesetter_newtabdiagram(fp, tab);
                        x = g_ps_ctab.cx;
                    }

                    pstypesetter_pinch_hammer_on_pull_off(fp, x, pstypesetter_pinch_y(s, g_ps_ctab.cy) + 5);
                    break;

                case '~':
                    pstypesetter_pinch_vibrato(fp, x, pstypesetter_pinch_y(s, g_ps_ctab.cy) + 3);
                    x += PSTYPESETTER_CARRIAGE_STEP;
                    break;

                case 'b':
                    pointed = (s == 0 || tab->strings[s - 1][o] != 'b');
                    pstypesetter_pinch_bend(fp, x, pstypesetter_pinch_y(s, g_ps_ctab.cy) + 5, pointed);
                    break;

                case 'r':
                    pointed = (s == tab->string_nr - 1 || tab->strings[s + 1][o] != 'r');
                    pstypesetter_pinch_release_bend(fp, x, pstypesetter_pinch_y(s, g_ps_ctab.cy), pointed);
                    break;

                case '-':
                    break;

                case '|':
                    pstypesetter_vertbar(fp, x, g_ps_ctab.cy, tab->string_nr);
                    s = tab->string_nr + 1;
                    x -= PSTYPESETTER_CARRIAGE_STEP;
                    continue;

                default:
                    if (tab->strings[s][o] == '/' || tab->strings[s][o] == '\\') {
                        x += 1;

                        if (x >= g_ps_ctab.cxr) {
                            if ((cset.prefs & kTlpPrefsFretboardStyleNormal    ) ||
                                (cset.prefs & kTlpPrefsCloseTabToSave          ) ||
                                ((cset.prefs & kTlpPrefsFretboardStyleContinuous) && tab->next == NULL)) {
                                    pstypesetter_close_tab(fp, x, tab->string_nr);
                            }

                            pstypesetter_newtabdiagram(fp, tab);
                            x = g_ps_ctab.cx;
                        }
                    }

                    fprintf(fp, "%d %d moveto (%s%c) show\n", x, pstypesetter_pinch_y(s, g_ps_ctab.cy),
                                                            (tab->strings[s][o] == '\\') ? "\\" : "",
                                                                tab->strings[s][o]);

                    if (tab->strings[s][o] == '/' || tab->strings[s][o] == '\\') {
                        x -= 1;
                    }
                    break;
            }

        }

        x += PSTYPESETTER_CARRIAGE_STEP;


        if (tab->techniques != NULL) {
            if (o > 0 && tab->techniques->data != NULL &&
                tab->techniques->data[o] != '.' && tab->techniques->data[o-1] == '.') {
                x += PSTYPESETTER_CARRIAGE_STEP;
            }
        }

        if (x >= g_ps_ctab.cxr) {
            if ((cset.prefs & kTlpPrefsFretboardStyleNormal    ) ||
                (cset.prefs & kTlpPrefsCloseTabToSave          ) ||
                ((cset.prefs & kTlpPrefsFretboardStyleContinuous) && tab->next == NULL)) {
                pstypesetter_close_tab(fp, x, tab->string_nr);
            }

            pstypesetter_newtabdiagram(fp, tab);
            x = g_ps_ctab.cx;
        }
    }

    if ((cset.prefs & kTlpPrefsFretboardStyleNormal    ) ||
        (cset.prefs & kTlpPrefsCloseTabToSave          ) ||
        ((cset.prefs & kTlpPrefsFretboardStyleContinuous) && tab->next == NULL)) {
        pstypesetter_close_tab(fp, x, tab->string_nr);
    }

    g_ps_ctab.cx = PSTYPESETTER_CARRIAGEX;

    g_ps_ctab.cy += PSTYPESETTER_NEXT_TABCHUNK;
}

static void pstypesetter_spill_song_title(FILE *fp, const char *song) {
    if (song == NULL) {
        return;
    }
    fprintf(fp, "/Times-Bold 20 selectfont\n");
    fprintf(fp, "%d %d moveto (%s) show\n", PSTYPESETTER_PAGEXL, g_ps_ctab.cy, song);
    fprintf(fp, "/Times-Bold 11 selectfont\n");
    g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;
}

static void pstypesetter_spill_transcribers_name(FILE *fp, const char *transcriber) {
    if (transcriber == NULL) {
        return;
    }
    fprintf(fp, "/Times-Bold-Italic 11 selectfont\n");
    fprintf(fp, "%d %d moveto (transcribed by %s) show\n", PSTYPESETTER_PAGEXL, g_ps_ctab.cy, transcriber);
    fprintf(fp, "/Times-Bold 11 selectfont\n");
    g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO * 3;
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

    g_ps_ctab.cy -= PSTYPESETTER_NEXT_ADDINFO;

    get_all_used_techniques(song, used_techniques, &used_techniques_nr, &has_muffled, &has_anyfret);

    if (used_techniques_nr > 0 || has_muffled || has_anyfret) {
        for (u = 0; u < used_techniques_nr; u++) {
            switch (used_techniques[u]) {
                case kTlpHammerOn:
                case kTlpPullOff:
                    if (!hp_done) {
                        hp_done = 1;
                        pstypesetter_pinch_hammer_on_pull_off(fp, PSTYPESETTER_CARRIAGEX + 20, g_ps_ctab.cy);
                        fprintf(fp, "%d %d moveto (=   %s/%s) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25,
                                                                     g_ps_ctab.cy,
                                                                     get_technique_notation_label(kTlpHammerOn),
                                                                     get_technique_notation_label(kTlpPullOff));
                        g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;
                    }
                    continue;

                case kTlpVibrato:
                    pstypesetter_pinch_vibrato(fp, PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.cy);
                    fprintf(fp, "%d %d moveto (=   %s) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25, g_ps_ctab.cy,
                                                      get_technique_notation_label(kTlpVibrato));
                    g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;
                    continue;

                case kTlpBend:
                    g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;
                    pstypesetter_pinch_bend(fp, PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.cy, 1);
                    fprintf(fp, "%d %d moveto (=   %s) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25, g_ps_ctab.cy,
                                                      get_technique_notation_label(kTlpBend));
                    g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;
                    continue;

                case kTlpReleaseBend:
                    pstypesetter_pinch_release_bend(fp, PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.cy, 1);
                    fprintf(fp, "%d %d moveto (=   %s) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25, g_ps_ctab.cy - 10,
                                                      get_technique_notation_label(kTlpReleaseBend));
                    g_ps_ctab.cy += (2 * PSTYPESETTER_NEXT_ADDINFO);
                    continue;

                case kTlpSlideUp:
                        fprintf(fp, "%d %d moveto (\\\\) show\n", PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.cy);
                    break;

                default:
                    fprintf(fp, "%d %d moveto (%s) show\n", PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.cy, get_technique_label(used_techniques[u]));
                    break;
            }

            fprintf(fp, "%d %d moveto (=   %s) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25, g_ps_ctab.cy,
                                                      get_technique_notation_label(used_techniques[u]));


            g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;
        }

        if (has_muffled) {
            fprintf(fp, "%d %d moveto (X) show\n", PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.cy);
            fprintf(fp, "%d %d moveto (=   Muffled note) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25, g_ps_ctab.cy);
            g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;
        }

        if (has_anyfret) {
            fprintf(fp, "%d %d moveto (?) show\n", PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.cy);
            fprintf(fp, "%d %d moveto (=   From any fret) show\n", PSTYPESETTER_CARRIAGEX + PSTYPESETTER_CARRIAGE_STEP + 25, g_ps_ctab.cy);
            g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;
        }
    }

    g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;

}

static void pstypesetter_spill_tunning(FILE *fp) {
    int t = 0;
    size_t t_nr = 0;
    char **tunning = NULL;
    struct typesetter_curr_settings cset = typesetter_settings();

    if ((cset.prefs & kTlpPrefsShowTunning) && !(cset.prefs & kTlpPrefsAddTunningToTheFretboard)) {

        if (!(cset.prefs & kTlpPrefsIncludeTabNotation)) {
            g_ps_ctab.cy -= PSTYPESETTER_NEXT_ADDINFO;
        }

        tunning = get_processor_setting("tunning", &t_nr);

        fprintf(fp, "%d %d moveto (Tunning [%d-1] = ", PSTYPESETTER_CARRIAGEX + 10, g_ps_ctab.cy, t_nr);

        for (t = t_nr - 1; t >= 0; t--) {
            fprintf(fp, "%s", tunning[t]);
            if (t != 0) {
                fprintf(fp, ", ");
            }
        }

        fprintf(fp, ") show\n");

        g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;

    }

    g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;
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
        g_ps_ctab.cy += (2 * PSTYPESETTER_NEXT_ADDINFO);
    }

    if (g_ps_ctab.cy < (PSTYPESETTER_PAGEY_LIMIT + 20)) {
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
            fprintf(fp, "%d %d moveto (%s) show\n", PSTYPESETTER_PAGEXL, g_ps_ctab.cy, comment);
            g_ps_ctab.cy += PSTYPESETTER_ADDINFO_LINEBREAK;
            dp++;
        }
    }

    fprintf(fp, "/Times-Bold 11 selectfont\n");

    g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;
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
            fprintf(fp, "%d %d moveto (%c) show\n", x, g_ps_ctab.cy, *tp);
        }

        for (s = 0; s < tab->string_nr; s++) {
            switch (tab->strings[s][tp_end - tp]) {
                case 'h':
                case 'p':
                    x += PSTYPESETTER_CARRIAGE_STEP + 10;
                    continue;

                case '~':
                    x += PSTYPESETTER_CARRIAGE_STEP;
                    continue;
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

    g_ps_ctab.cy += PSTYPESETTER_NEXT_STRING;

}

static void pstypesetter_spill_sustained_techniques(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    const txttypesetter_sustained_technique_ctx *tp = NULL;
    int has_half_step_notes = tunning_has_half_step_notes(tab, NULL, typesetter_settings().prefs);
    int x = PSTYPESETTER_CARRIAGEX;
    char *dp = NULL, *dp_end = NULL;
    size_t s = 0;
    int alpha_has_printed = 0;

    if (is_tab_empty(tab)) {
        return;
    }

    if (tab->comments == NULL) {
        g_ps_ctab.cy += (2 * PSTYPESETTER_NEXT_ADDINFO);
    }

    if (g_ps_ctab.cy < (PSTYPESETTER_PAGEY_LIMIT + 60)) {
        pstypesetter_pageinit();
        pstypesetter_showpage(fp);
        pstypesetter_newpage(fp);
    }
    g_ps_ctab_sustained_techniques_y = g_ps_ctab.cy;

    for (tp = tab->techniques; tp != NULL; tp = tp->next) {
        if (has_half_step_notes) {
            x += PSTYPESETTER_CARRIAGE_STEP;
        }

        dp = &tp->data[0];
        dp_end = dp + strlen(dp) - 1;

        alpha_has_printed = 0;
        while (dp != dp_end) {
            fprintf(fp, "%d %d moveto ", x, g_ps_ctab.cy);

            if ((*dp != '.' && *dp != ' ') || dp == &tp->data[0]) {
                fprintf(fp, "(%c) show\n", *dp);
                if (!alpha_has_printed) {
                    alpha_has_printed = isalpha(*(dp));
                }
            } else {
                if (alpha_has_printed && (*(dp - 1) == '.' || *(dp + 1) == '.')) { // WARN(Rafael): Yes, just accept it.
                    fprintf(fp, "(-) show\n");
                }
            }

            for (s = 0; s < tab->string_nr; s++) {
                switch (tab->strings[s][dp_end - dp]) {
                    case '|':
                        if (!alpha_has_printed) {
                            x -= PSTYPESETTER_CARRIAGE_STEP;
                            s = tab->string_nr + 1;
                        }
                        break;

                    case 'h':
                    case 'p':
                        if (!alpha_has_printed) {
                            x += PSTYPESETTER_CARRIAGE_STEP + 10;
                            s = tab->string_nr + 1;
                        }
                        break;

                    case '~':
                        if (!alpha_has_printed) {
                            x += PSTYPESETTER_CARRIAGE_STEP;
                            s = tab->string_nr + 1;
                        }
                        break;
                }
            }

            if (isalpha(*dp) && *(dp + 1) == '.') {
                x += 5;
            }
            x += PSTYPESETTER_CARRIAGE_STEP;
            dp++;
        }

        g_ps_ctab.cy += PSTYPESETTER_ADDINFO_LINEBREAK;
        x = PSTYPESETTER_CARRIAGEX;
    }

    g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;
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
