/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/txtinkspill.h>
#include <processor/settings.h>
#include <base/types.h>
#include <dsl/utils.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static void txttypesetter_spill_comments(FILE *fp, const txttypesetter_comment_ctx *comments);

static void txttypesetter_spill_sustained_techniques(FILE *fp, const txttypesetter_sustained_technique_ctx *techniques);

static void txttypesetter_spill_fretboard_pinches(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void txttypesetter_spill_times(FILE *fp, const char *times);

static void txttypesetter_spill_song_title(FILE *fp, const char *song);

static void txttypesetter_spill_transcribers_name(FILE *fp, const char *transcriber);

static void txttypesetter_init_settings();

struct txttypesetter_curr_settings {
    tulip_prefs_map_t prefs;
    size_t indentation_deepness;
};

static struct txttypesetter_curr_settings g_txttypesetter_settings;

static void txttypesetter_init_settings() {
    void *data = NULL;

    g_txttypesetter_settings.prefs = 0;

    data = get_processor_setting("indentation-deepness", NULL);

    g_txttypesetter_settings.indentation_deepness = (data == NULL) ? 0 : *(size_t *)data;

    data = get_processor_setting("close-tab-to-save", NULL);
    if (data != NULL) {
        g_txttypesetter_settings.prefs = *(tulip_prefs_map_t *)data;
    }
}

static void txttypesetter_spill_fretboard_pinches(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    size_t s = 0;
    size_t i = 0;
    ssize_t s_limit = -1, real_s_limit = -1;
    txttypesetter_sustained_technique_ctx *tp = NULL;

    if (g_txttypesetter_settings.prefs & kTlpPrefsCutTabOnTheLastNote) {
        for (s = 0; s < tab->string_nr; s++) {

            for (i = strlen(tab->strings[s]) - 1; i >= 0 && is_sep(tab->strings[s][i]); i--);

            i++;

            if (!is_sep(tab->strings[s][i - 1]) && !is_sep_bar(tab->strings[s][i - 1])) {
                i++;
            } else if (is_sep_bar(tab->strings[s][i - 1])) {
                i--;
            }

            if ((ssize_t)i > s_limit) {
                s_limit = (ssize_t)i;
            }
        }
    }

    for (s = 0; s < tab->string_nr; s++) {

        for (i = 0; i < g_txttypesetter_settings.indentation_deepness; i++) {
            fprintf(fp, " ");
        }

        if (g_txttypesetter_settings.prefs & kTlpPrefsShowTunning) {
            fprintf(fp, "%s", tab->tunning[s]);
        } else {
            fprintf(fp, "|");
        }

        if (s_limit == -1) {
            fprintf(fp, "%s", tab->strings[s]);
        } else {
            real_s_limit = s_limit;
            for (tp = tab->techniques; tp != NULL; tp = tp->next) {
                if (tp->data == NULL) {
                    continue;
                }
                while (is_sustain(tp->data[real_s_limit])) {
                    real_s_limit++;
                }
            }
            fwrite(tab->strings[s], 1, real_s_limit, fp);
        }

        if ((g_txttypesetter_settings.prefs  & kTlpPrefsFretboardStyleNormal    ) ||
            (g_txttypesetter_settings.prefs  & kTlpPrefsCloseTabToSave          ) ||
            ((g_txttypesetter_settings.prefs & kTlpPrefsFretboardStyleContinuous) && tab->next == NULL)) {
            fprintf(fp, "|");
        }

        fprintf(fp, "\n");

    }
    fprintf(fp, "\n");
}

static void txttypesetter_spill_sustained_techniques(FILE *fp, const txttypesetter_sustained_technique_ctx *techniques) {
    const txttypesetter_sustained_technique_ctx *tp = NULL;
    size_t i = 0;

    for (tp = techniques; tp != NULL; tp = tp->next) {
        for (i = 0; i < g_txttypesetter_settings.indentation_deepness; i++) {
            fprintf(fp, " ");
        }
        fprintf(fp, "%s\n", tp->data);
    }

    if (techniques != NULL) {
        fprintf(fp, "\n");
    }
}

static void txttypesetter_spill_comments(FILE *fp, const txttypesetter_comment_ctx *comments) {
    const txttypesetter_comment_ctx *cp = NULL;

    for (cp = comments; cp != NULL; cp = cp->next) {
        fprintf(fp, "%s\n", cp->data);
    }

    if (comments != NULL) {
        fprintf(fp, "\n");
    }
}

static void txttypesetter_spill_times(FILE *fp, const char *times) {
    const char *tp = times;
    int print_times = 0;
    size_t i = 0;

    if (tp == NULL) {
        return;
    }

    while (*tp != 0 && !print_times) {
        print_times = isdigit(*tp);
        tp++;
    }

    if (print_times) {
        for (i = 0; i < g_txttypesetter_settings.indentation_deepness; i++) {
            fprintf(fp, " ");
        }
        fprintf(fp, "%s\n", times);
    }
}

static void txttypesetter_spill_song_title(FILE *fp, const char *song) {
    if (song == NULL) {
        return;
    }
    fprintf(fp, "Song: %s\n", song);
}

static void txttypesetter_spill_transcribers_name(FILE *fp, const char *transcriber) {
    if (transcriber == NULL) {
        return;
    }
    fprintf(fp, "Transcribed by: %s\n\n", transcriber);
}

int txttypesetter_inkspill(const char *filepath, const txttypesetter_tablature_ctx *tab) {
    FILE *fp = NULL;
    const txttypesetter_tablature_ctx *tp = NULL;

    if (filepath == NULL || tab == NULL) {
        return 0;
    }

    fp = fopen(filepath, "w");

    if (fp == NULL) {
        return 0;
    }

    txttypesetter_init_settings();

    txttypesetter_spill_song_title(fp, tab->song);
    txttypesetter_spill_transcribers_name(fp, tab->transcriber);
    if (tab->song != NULL && tab->transcriber == NULL) {
        fprintf(fp, "\n");
    }

    for (tp = tab; tp != NULL; tp = tp->next) {
        txttypesetter_spill_comments(fp, tp->comments);
        txttypesetter_spill_sustained_techniques(fp, tp->techniques);
        txttypesetter_spill_times(fp, tp->times);
        txttypesetter_spill_fretboard_pinches(fp, tp);
    }

    fclose(fp);

    return 1;
}
