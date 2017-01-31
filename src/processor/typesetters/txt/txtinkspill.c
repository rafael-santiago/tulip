/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/txtinkspill.h>
#include <processor/settings.h>
#include <processor/oututils.h>
#include <dsl/utils.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define is_relevant_technique(t) ( (t) == kTlpHammerOn           ||\
                                   (t) == kTlpPullOff            ||\
                                   (t) == kTlpVibrato            ||\
                                   (t) == kTlpSlideDown          ||\
                                   (t) == kTlpSlideUp            ||\
                                   (t) == kTlpBend               ||\
                                   (t) == kTlpReleaseBend        ||\
                                   (t) == kTlpTapping            ||\
                                   (t) == kTlpNaturalHarmonic    ||\
                                   (t) == kTlpArtificialHarmonic ||\
                                   (t) == kTlpMute               ||\
                                   (t) == kTlpLetRing            ||\
                                   (t) == kTlpStrum              ||\
                                   (t) == kTlpTremoloPicking     ||\
                                   (t) == kTlpVibratoWBar        ||\
                                   (t) == kTlpTrill )

static void txttypesetter_spill_comments(FILE *fp, const txttypesetter_comment_ctx *comments);

static void txttypesetter_spill_sustained_techniques(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void txttypesetter_spill_fretboard_pinches(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void txttypesetter_spill_times(FILE *fp, const char *times, const char tunning[6][4]);

static void txttypesetter_spill_song_title(FILE *fp, const char *song);

static void txttypesetter_spill_transcribers_name(FILE *fp, const char *transcriber);

static void txttypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song);

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

    g_txttypesetter_settings.indentation_deepness = ( data == NULL) ? 0 : *(size_t *)data;

    data = get_processor_setting("prefs", NULL);
    if (data != NULL) {
        g_txttypesetter_settings.prefs = *(tulip_prefs_map_t *)data;
    }
}

static void txttypesetter_spill_fretboard_pinches(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    size_t s = 0;
    size_t i = 0;
    ssize_t s_limit = -1, real_s_limit = -1;
    txttypesetter_sustained_technique_ctx *tp = NULL;

    if (is_tab_empty(tab)) {
        return;
    }

    if (g_txttypesetter_settings.prefs & kTlpPrefsCutTabOnTheLastNote) {
        for (s = 0; s < tab->string_nr; s++) {

            for (i = strlen(tab->strings[s]) - 1; i > 0 && is_sep(tab->strings[s][i]); i--);

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

static void txttypesetter_spill_sustained_techniques(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    const txttypesetter_sustained_technique_ctx *tp = NULL;
    size_t i = 0;
    int has_half_step_notes = tunning_has_half_step_notes(tab, NULL, g_txttypesetter_settings.prefs);

    for (tp = tab->techniques; tp != NULL; tp = tp->next) {
        for (i = 0; i < g_txttypesetter_settings.indentation_deepness; i++) {
            fprintf(fp, " ");
        }
        fprintf(fp, " ");
        if (has_half_step_notes) {
            fprintf(fp, " ");
        }
        for (i = 1; i < tab->fretboard_sz; i++) {
            if (tab->strings[0][i] == '|' && tp->data[i] == '.') {
                tp->data[i] = (tp->data[i+1] == '.') ? ' ' : 0;
            }
        }
        fprintf(fp, "%s\n", tp->data);
    }

    if (tab->techniques != NULL) {
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

static void txttypesetter_spill_times(FILE *fp, const char *times, const char tunning[6][4]) {
    const char *tp = times;
    int print_times = 0;
    size_t i = 0;
    int has_half_step_notes = tunning_has_half_step_notes(NULL, tunning, g_txttypesetter_settings.prefs);

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
        fprintf(fp, " ");
        if (has_half_step_notes) {
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

static void txttypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song) {
    tulip_command_t used_techniques[31];
    size_t used_techniques_nr = 0, u = 0;
    tulip_command_t *demuxes = NULL;
    size_t demuxes_nr = 0, d = 0;
    const tulip_single_note_ctx *sp = NULL;
    int found = 0, s = 0;
    char **tunning = NULL;
    int has_muffled = 0;
    int has_anyfret = 0;

    if ((g_txttypesetter_settings.prefs & kTlpPrefsIncludeTabNotation) == 0) {
        return;
    }

    if (song == NULL) {
        return;
    }

    for (sp = song; sp != NULL && used_techniques_nr < sizeof(used_techniques); sp = sp->next) {
        demuxes = demux_tlp_commands(sp->techniques, &demuxes_nr);

        for (d = 0; d < demuxes_nr; d++) {
            if (is_relevant_technique(demuxes[d])) {
                found = 0;
                for (u = 0; u < used_techniques_nr && !found; u++) {
                    found = (demuxes[d] == used_techniques[u]);
                }
                if (!found) {
                    used_techniques[used_techniques_nr] = demuxes[d];
                    used_techniques_nr++;
                }
            } else if (sp->buf[0] != 0) {
                if (!has_muffled) {
                    has_muffled = (strstr(sp->buf, "X") != NULL);
                }
                if (!has_anyfret) {
                    has_anyfret = (strstr(sp->buf, "?") != NULL);
                }
            }
        }

        free(demuxes);
    }

    if (used_techniques_nr > 0) {
        for (u = 0; u < used_techniques_nr; u++) {
            fprintf(fp, "%s = %s\n", get_technique_label(used_techniques[u]),
                                     get_technique_notation_label(used_techniques[u]));

        }
        //  WARN(Santiago): These "techniques" are just single note complements. Then, we can handle it
        //                  from here.
        if (has_muffled) {
            fprintf(fp, "X = Muffled note\n");
        }
        if (has_anyfret) {
            fprintf(fp, "? = From any fret\n");
        }
        fprintf(fp, "\n");
    }

    if ((g_txttypesetter_settings.prefs & kTlpPrefsShowTunning) == 0) {
        tunning = get_processor_setting("tunning", &d);

        fprintf(fp, "Tunning [%d-1]: ", d);

        for (s = d - 1; s >= 0; s--) {
            fprintf(fp, "%c", tunning[s][0]);

            if (tunning[s][1] != 0 && tunning[s][1] != ' ') {
                fprintf(fp, "%c", tunning[s][1]);
            }

            if (s != 0) {
                fprintf(fp, ", ");
            } else {
                fprintf(fp, "\n");
            }
        }

        fprintf(fp, "\n");
    }

}

int txttypesetter_inkspill(const char *filepath, const txttypesetter_tablature_ctx *tab, const tulip_single_note_ctx *song) {
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

    txttypesetter_spill_tab_notation(fp, song);

    for (tp = tab; tp != NULL; tp = tp->next) {
        txttypesetter_spill_comments(fp, tp->comments);
        txttypesetter_spill_sustained_techniques(fp, tp);
        txttypesetter_spill_times(fp, tp->times, tp->tunning);
        txttypesetter_spill_fretboard_pinches(fp, tp);
    }

    fclose(fp);

    return 1;
}
