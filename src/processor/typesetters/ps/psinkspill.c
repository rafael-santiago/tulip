/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/ps/psinkspill.h>
#include <processor/typesetters/ps/psboundaries.h>
#include <processor/oututils.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int g_ps_cpage = 0;

struct pstypesetters_addinfo_y_ctx {
    int song;
    int transcriber;
    int tabnotation;
    int comments;
    int sustained_techniques;
    int times;
}; // WARN(Rafael): --DEPRECATED BULLSHIT--

struct pstypesetter_tab_diagram_ctx {
    int cxl, cxr;
    int cy, cx;
    struct pstypesetters_addinfo_y_ctx addinfo_y; // WARN(Rafael): --DEPRECATED BULLSHIT--
};

typedef enum _pstypesetter_addinfo { // WARN(Rafael): --DEPRECATED BULLSHIT--
    kSong,
    kTranscriber,
    kTabNotation,
    kComments,
    kSustainedTechniques,
    kTimes
}pstypesetter_addinfo_t;

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

static int pstypesetter_addinfo_y(const pstypesetter_addinfo_t addinfo); // WARN(Rafael): --DEPRECATED BULLSHIT--

static void pstypesetter_spill_sustained_techniques(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void pstypesetter_spill_times(FILE *fp, const char *times, const char tunning[6][4]);

static void pstypesetter_spill_comments(FILE *fp, const txttypesetter_comment_ctx *comments);

static void pstypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song);

static void pstypesetter_spill_transcribers_name(FILE *fp, const char *transcriber);

static void pstypesetter_spill_song_title(FILE *fp, const char *song);

static void pstypesetter_init(void) {
    g_ps_cpage = 0;
}

static void pstypesetter_pageinit(void) {
    g_ps_ctab.cxl = PSTYPESETTER_PAGEXL;
    g_ps_ctab.cxr = PSTYPESETTER_PAGEXR;
    g_ps_ctab.cy = PSTYPESETTER_PAGEY;
    memset(&g_ps_ctab.addinfo_y, 0, sizeof(g_ps_ctab.addinfo_y));
}

static FILE *pstypesetter_newps(const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (fp == NULL) {
        return NULL;
    }
    fprintf(fp, "%%!PS-Adobe-3.0\n"
                "/Times-Bold 11 selectfont\n"
                "%.1f setlinewidth\n", PSTYPESETTER_TABLINE_W);
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

static int pstypesetter_addinfo_y(const pstypesetter_addinfo_t addinfo) { // WARN(Rafael): --DEPRECATED BULLSHIT--
    int y = g_ps_ctab.cy;
    int n = 0;
    int *sv = &n;

    switch (addinfo) {
        case kSong:
            sv = &g_ps_ctab.addinfo_y.song;
            break;

        case kTranscriber:
            n = g_ps_ctab.addinfo_y.song;
            sv = &g_ps_ctab.addinfo_y.transcriber;
            break;

        case kTabNotation:
            if (g_ps_ctab.addinfo_y.transcriber > 0) {
                n = g_ps_ctab.addinfo_y.transcriber;
            } else {
                n = g_ps_ctab.addinfo_y.song;
            }
            sv = &g_ps_ctab.addinfo_y.tabnotation;
            break;

        case kComments:
            if (g_ps_ctab.addinfo_y.tabnotation > 0) {
                n = g_ps_ctab.addinfo_y.tabnotation;
            } else if (g_ps_ctab.addinfo_y.transcriber > 0) {
                n = g_ps_ctab.addinfo_y.transcriber;
            } else {
                n = g_ps_ctab.addinfo_y.song;
            }
            sv = &g_ps_ctab.addinfo_y.comments;
            break;

        case kSustainedTechniques:
            if (g_ps_ctab.addinfo_y.comments > 0) {
                n = g_ps_ctab.addinfo_y.comments;
            } else if (g_ps_ctab.addinfo_y.tabnotation > 0) {
                n = g_ps_ctab.addinfo_y.tabnotation;
            } else if  (g_ps_ctab.addinfo_y.transcriber > 0) {
                n = g_ps_ctab.addinfo_y.transcriber;
            } else {
                n = g_ps_ctab.addinfo_y.song;
            }
            sv = &g_ps_ctab.addinfo_y.sustained_techniques;
            break;

        case kTimes:
            if (g_ps_ctab.addinfo_y.sustained_techniques > 0) {
                n = g_ps_ctab.addinfo_y.sustained_techniques;
            } else if (g_ps_ctab.addinfo_y.comments > 0) {
                n = g_ps_ctab.addinfo_y.comments;
            } else if (g_ps_ctab.addinfo_y.tabnotation > 0) {
                n = g_ps_ctab.addinfo_y.tabnotation;
            } else if (g_ps_ctab.addinfo_y.transcriber > 0) {
                n = g_ps_ctab.addinfo_y.transcriber;
            } else {
                n = g_ps_ctab.addinfo_y.song;
            }
            sv = &g_ps_ctab.addinfo_y.times;
            break;

    }

    n += PSTYPESETTER_NEXT_ADDINFO;
    *sv = y;

    return *sv;
}

static void pstypesetter_newtabdiagram(FILE *fp, const int sn) {
    int s = 0;
    int sy = 0;

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

    fprintf(fp, "stroke\n");
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

    pstypesetter_spill_comments(fp, tab->comments);
    pstypesetter_spill_sustained_techniques(fp, tab);
    pstypesetter_spill_times(fp, tab->times, tab->tunning);

    pstypesetter_newtabdiagram(fp, tab->string_nr);

    pstypesetter_flush_fretboard_pinches(fp, tab);
}

static void pstypesetter_vertbar(FILE *fp, const int x, const int y) {
    fprintf(fp, "%d %d moveto\n"
                "%d %d lineto\n"
                "stroke\n", x, pstypesetter_string_y(0, y), x, pstypesetter_string_y(5, y));
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
                    pstypesetter_vertbar(fp, x, g_ps_ctab.cy);
                    s = tab->string_nr + 1;
                    x -= PSTYPESETTER_CARRIAGE_STEP;
                    continue;

                default:
                    fprintf(fp, "%d %d moveto (%c) show\n", x,
                        pstypesetter_pinch_y(s, g_ps_ctab.cy),
                                           tab->strings[s][o]);
                    break;
            }

        }

        x += PSTYPESETTER_CARRIAGE_STEP;

        if (x >= g_ps_ctab.cxr) {
            pstypesetter_newtabdiagram(fp, tab->string_nr);
        }
    }

    g_ps_ctab.cx = PSTYPESETTER_CARRIAGEX;
}

static void pstypesetter_spill_song_title(FILE *fp, const char *song) {
    if (song == NULL) {
        return;
    }
    fprintf(fp, "%d %d moveto (%s) show\n", PSTYPESETTER_PAGEXL, pstypesetter_addinfo_y(kSong), song);
}

static void pstypesetter_spill_transcribers_name(FILE *fp, const char *transcriber) {
    if (transcriber == NULL) {
        return;
    }
    fprintf(fp, "%d %d moveto (%s) show\n", PSTYPESETTER_PAGEXL, pstypesetter_addinfo_y(kTranscriber), transcriber);
}

static void pstypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song) {
    // TODO(Rafael): Later.
}

static void pstypesetter_spill_comments(FILE *fp, const txttypesetter_comment_ctx *comments) {
    const txttypesetter_comment_ctx *cp = NULL;

    if (comments == NULL) {
        return;
    }

    for (cp = comments; cp != NULL; cp = cp->next) {
        fprintf(fp, "%d %d moveto (%s) show\n", PSTYPESETTER_PAGEXL, g_ps_ctab.cy, cp->data);
        g_ps_ctab.cy += PSTYPESETTER_ADDINFO_LINEBREAK;
    }

    g_ps_ctab.cy += PSTYPESETTER_NEXT_ADDINFO;
}

static void pstypesetter_spill_times(FILE *fp, const char *times, const char tunning[6][4]) {
    // TODO(Rafael): Later.
}

static void pstypesetter_spill_sustained_techniques(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    const txttypesetter_sustained_technique_ctx *tp = NULL;
    //int has_half_step_notes = tunning_has_half_step_notes(tab, NULL, 0);  Not yet coming soon.
    int has_half_step_notes = 0;
    int x = PSTYPESETTER_CARRIAGEX, y = g_ps_ctab.cy;
    char *dp = NULL, *dp_end = NULL;
    size_t s = 0;

    for (tp = tab->techniques; tp != NULL; tp = tp->next) {
        if (has_half_step_notes) {
            x += PSTYPESETTER_CARRIAGE_STEP;
        }

        dp = &tp->data[0];
        dp_end = dp + strlen(dp);

        while (dp != dp_end) {
            fprintf(fp, "%d %d moveto ", x, g_ps_ctab.cy);

            if ((*dp != '.' && *dp != ' ') || dp == &tp->data[0] || isalpha(*(dp-1))) {
                fprintf(fp, "(%c) show\n", *dp);
            } else {
                fprintf(fp, "(-) show\n");
            }


            for (s = 0; s < tab->string_nr; s++) {
                if (tab->strings[s][dp_end - dp] == '|') {
                    x -= PSTYPESETTER_CARRIAGE_STEP;
                    break;
                }
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

    pstypesetter_spill_song_title(fp, tab->song);
    pstypesetter_spill_transcribers_name(fp, tab->transcriber);

    pstypesetter_spill_tab_notation(fp, song);

    for (tp = tab; tp != NULL; tp = tp->next) {
        pstypesetter_proc_tabchunk(fp, tp);
    }

    pstypesetter_closeps(fp);

    return 1;
}
