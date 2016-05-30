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

static void txttypesetter_spill_comments(FILE *fp, const txttypesetter_comment_ctx *comments);

static void txttypesetter_spill_sustained_techniques(FILE *fp, const txttypesetter_sustained_technique_ctx *techniques);

static void txttypesetter_spill_fretboard_pinches(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void txttypesetter_init_settings();

struct txttypesetter_curr_settings {
    tulip_prefs_map_t prefs;
    size_t indentation_deepness;
};

static struct txttypesetter_curr_settings g_txttypesetter_settings;

static void txttypesetter_init_settings() {
    // TODO(Santiago): Load the settings.
}

static void txttypesetter_spill_fretboard_pinches(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    size_t s = 0;
    size_t i = 0;
    ssize_t s_limit = -1;

    if (g_txttypesetter_settings.prefs & kTlpPrefsCutTabOnTheLastNote) {
        for (s = 0; s < tab->string_nr; s++) {
            for (i = strlen(tab->strings[s]) - 1; i >= 0 && is_sep(tab->strings[s][i]); i--);
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
            fwrite(tab->strings[s], 1, s_limit, fp);
        }

        if ((g_txttypesetter_settings.prefs  & kTlpPrefsFretboardStyleNormal    ) ||
            (g_txttypesetter_settings.prefs  & kTlpPrefsCloseTabToSave          ) ||
            ((g_txttypesetter_settings.prefs & kTlpPrefsFretboardStyleContinuous) && tab->next == NULL)) {
            fprintf(fp, "|");
        }

        fprintf(fp, "\n");

    }
}

static void txttypesetter_spill_sustained_techniques(FILE *fp, const txttypesetter_sustained_technique_ctx *techniques) {
    const txttypesetter_sustained_technique_ctx *tp = NULL;
    for (tp = techniques; tp != NULL; tp = tp->next) {
        fprintf(fp, "%s\n", tp->data);
    }
    fprintf(fp, "\n");
}

static void txttypesetter_spill_comments(FILE *fp, const txttypesetter_comment_ctx *comments) {
    const txttypesetter_comment_ctx *cp = NULL;
    for (cp = comments; cp != NULL; cp = cp->next) {
        fprintf(fp, "%s\n", cp->data);
    }
    fprintf(fp, "\n");
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
    //  TODO(Santiago): Handle the minor things like song title, author name, etc.
    for (tp = tab; tp != NULL; tp = tp->next) {
        txttypesetter_spill_comments(fp, tp->comments);
        txttypesetter_spill_sustained_techniques(fp, tp->techniques);
        fprintf(fp, "%s\n", tp->times);
        txttypesetter_spill_fretboard_pinches(fp, tp);
    }
    fclose(fp);
    return 1;
}
