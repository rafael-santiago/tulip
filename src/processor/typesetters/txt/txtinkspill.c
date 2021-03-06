/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/txtinkspill.h>
#include <processor/typesetters/typeprefs.h>
#include <processor/settings.h>
#include <processor/oututils.h>
#include <dsl/utils.h>
#include <base/memory.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static void txttypesetter_spill_comments(FILE *fp, const txttypesetter_comment_ctx *comments);

static void txttypesetter_spill_sustained_techniques(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void txttypesetter_spill_fretboard_pinches(FILE *fp, const txttypesetter_tablature_ctx *tab);

static void txttypesetter_spill_times(FILE *fp, const char *times, const char tuning[6][4]);

static void txttypesetter_spill_song_title(FILE *fp, const char *song);

static void txttypesetter_spill_transcribers_name(FILE *fp, const char *transcriber);

static void txttypesetter_spill_tuning(FILE *fp);

static void txttypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song);

static void txttypesetter_spill_fretboard_pinches(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    size_t s = 0;
    size_t i = 0;
    ssize_t s_limit = -1;
    txttypesetter_sustained_technique_ctx *tp = NULL;
    struct typesetter_curr_settings cset;
    int half_step_notes = 0;

    if (is_tab_empty(tab, -1)) {
        return;
    }

    cset = typesetter_settings();

    if (cset.prefs & kTlpPrefsCutTabOnTheLastNote) {
        s_limit = get_fretboard_usage_limit(tab);
    }

    half_step_notes = tuning_has_half_step_notes(tab, NULL, cset.prefs);

    for (s = 0; s < tab->string_nr; s++) {

        for (i = 0; i < cset.indentation_deepness; i++) {
            fprintf(fp, " ");
        }

        if (cset.prefs & kTlpPrefsAddTuningToTheFretboard) {
            fprintf(fp, "%s", tab->tuning[s]);
            if (half_step_notes && strlen(tab->tuning[s]) == 1) {
                fprintf(fp, " ");
            }
        } else {
            fprintf(fp, "|");
        }

        if (s_limit == -1 || tab->strings[s][s_limit - 1] == 0) {
            fprintf(fp, "%s", tab->strings[s]);
        } else {
            /*real_s_limit = s_limit;
            for (tp = tab->techniques; tp != NULL; tp = tp->next) {
                if (tp->data == NULL) {
                    continue;
                }
                while (is_sustain(tp->data[real_s_limit])) {
                    real_s_limit++;
                }
            }*/
            fwrite(tab->strings[s], 1, s_limit, fp);
        }

        if ((cset.prefs  & kTlpPrefsFretboardStyleNormal    ) ||
            (cset.prefs  & kTlpPrefsCloseTabToSave          ) ||
            ((cset.prefs & kTlpPrefsFretboardStyleContinuous) && tab->next == NULL)) {
            fprintf(fp, "|");
        }

        fprintf(fp, "\n");

    }
    fprintf(fp, "\n");
}

static void txttypesetter_spill_sustained_techniques(FILE *fp, const txttypesetter_tablature_ctx *tab) {
    const txttypesetter_sustained_technique_ctx *tp = NULL;
    size_t i = 0;
    struct typesetter_curr_settings cset = typesetter_settings();
    int has_half_step_notes = tuning_has_half_step_notes(tab, NULL, cset.prefs);
    ssize_t usage_limit = -1;

    if (is_tab_empty(tab, -1)) {
        return;
    }

    if (cset.prefs & kTlpPrefsCutTabOnTheLastNote) {
        usage_limit = get_fretboard_usage_limit(tab);
    }

    for (tp = tab->techniques; tp != NULL; tp = tp->next) {
        for (i = 0; i < cset.indentation_deepness; i++) {
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
        if (usage_limit > -1) {
            tp->data[usage_limit] = 0;
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

static void txttypesetter_spill_times(FILE *fp, const char *times, const char tuning[6][4]) {
    const char *tp = times;
    int print_times = 0;
    size_t i = 0;
    struct typesetter_curr_settings cset = typesetter_settings();
    int has_half_step_notes = tuning_has_half_step_notes(NULL, tuning, cset.prefs);

    if (tp == NULL) {
        return;
    }

    while (*tp != 0 && !print_times) {
        print_times = isdigit(*tp);
        tp++;
    }

    if (print_times) {
        for (i = 0; i < cset.indentation_deepness; i++) {
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

static void txttypesetter_spill_tuning(FILE *fp) {
    char **tuning = NULL;
    size_t d = 0;
    int s = 0;
    struct typesetter_curr_settings cset = typesetter_settings();

    if ((cset.prefs & kTlpPrefsShowTuning) && !(cset.prefs & kTlpPrefsAddTuningToTheFretboard)) {
        tuning = get_processor_setting("tuning", &d);

        fprintf(fp, "Tuning [%d-1]: ", d);

        for (s = d - 1; s >= 0; s--) {
            fprintf(fp, "%c", tuning[s][0]);

            if (tuning[s][1] != 0 && tuning[s][1] != ' ') {
                fprintf(fp, "%c", tuning[s][1]);
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

static void txttypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song) {
    tulip_command_t used_techniques[31];
    size_t used_techniques_nr = 0, u = 0;
    int s = 0;
    int has_muffled = 0;
    int has_anyfret = 0;
    struct typesetter_curr_settings cset = typesetter_settings();

    if ((cset.prefs & kTlpPrefsIncludeTabNotation) == 0) {
        return;
    }

    if (song == NULL) {
        return;
    }

    get_all_used_techniques(song, used_techniques, &used_techniques_nr, &has_muffled, &has_anyfret);

    if (used_techniques_nr > 0 || has_muffled || has_anyfret) {
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

    txttypesetter_spill_song_title(fp, tab->song);
    txttypesetter_spill_transcribers_name(fp, tab->transcriber);
    if (tab->song != NULL && tab->transcriber == NULL) {
        fprintf(fp, "\n");
    }

    txttypesetter_spill_tab_notation(fp, song);

    txttypesetter_spill_tuning(fp);

    for (tp = tab; tp != NULL; tp = tp->next) {
        txttypesetter_spill_comments(fp, tp->comments);
        txttypesetter_spill_sustained_techniques(fp, tp);
        if (!is_tab_empty(tp, -1)) {
            txttypesetter_spill_times(fp, tp->times, tp->tuning);
        }
        txttypesetter_spill_fretboard_pinches(fp, tp);
    }

    fclose(fp);

    return 1;
}
