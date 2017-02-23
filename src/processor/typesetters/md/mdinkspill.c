/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/md/mdinkspill.h>
#include <processor/typesetters/txt/txt.h>
#include <processor/typesetters/meta.h>
#include <processor/typesetters/typeprefs.h>
#include <processor/settings.h>
#include <processor/oututils.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void mdtypesetter_spill_song_title(FILE *fp, const char *title);

static void mdtypesetter_spill_transcribers_name(FILE *fp, const char *name);

static void mdtypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song);

static void mdtypesetter_spill_tuning(FILE *fp);

static void mdtypesetter_spill_song(FILE *fp, const tulip_single_note_ctx *song);

static void mdtypesetter_spill_song_title(FILE *fp, const char *title) {
    if (title == NULL) {
        return;
    }
    fprintf(fp, "# %s\n", title);
}

static void mdtypesetter_spill_transcribers_name(FILE *fp, const char *name) {
    if (name == NULL) {
        return;
    }

    fprintf(fp, "*transcribed by %s*\n", name);
}

static void mdtypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song) {
    struct typesetter_curr_settings cset = typesetter_settings();
    tulip_command_t used_techniques[31];
    size_t used_techniques_nr = 0, u = 0;
    int has_muffled = 0, has_anyfret = 0;
    int s = 0;

    if ((cset.prefs & kTlpPrefsIncludeTabNotation) == 0 || song == NULL) {
        return;
    }

    get_all_used_techniques(song, used_techniques, &used_techniques_nr, &has_muffled, &has_anyfret);

    if (used_techniques_nr > 0 || has_muffled || has_anyfret) {
        fprintf(fp, "|**Technique**|**Description**|\n"
                    "|:-----------:|:-------------:|\n");
        for (u = 0; u < used_techniques_nr; u++) {
            fprintf(fp, "|``%s``|%s|\n", get_technique_label(used_techniques[u]),
                                         get_technique_notation_label(used_techniques[u]));

        }

        if (has_muffled) {
            fprintf(fp, "|``X``|Muffled note|\n");
        }

        if (has_anyfret) {
            fprintf(fp, "|``?``|From any fret|\n");
        }

        fprintf(fp, "\n");
    }

}

static void mdtypesetter_spill_tuning(FILE *fp) {
    char **tuning = NULL;
    size_t d = 0;
    int s = 0;
    struct typesetter_curr_settings cset = typesetter_settings();

    if ((cset.prefs & kTlpPrefsShowTuning) && !(cset.prefs & kTlpPrefsAddTuningToTheFretboard)) {
        tuning = get_processor_setting("tuning", &d);

        fprintf(fp, "**Tuning [%d-1]**: ", d);

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

static void mdtypesetter_spill_song(FILE *fp, const tulip_single_note_ctx *song) {
    char *data = NULL;

    data = typesetter_raw_output(song, NULL, txt_typesetter);

    if (data != NULL) {
        fprintf(fp, "```\n"
                    "%s\n"
                    "```\n", data);
        free(data);
    }
}

int mdtypesetter_inkspill(const char *filepath, const tulip_single_note_ctx *song) {
    return metatypesetter_inkspill(filepath, song, "\n",
                                   mdtypesetter_spill_song_title,
                                   mdtypesetter_spill_transcribers_name,
                                   mdtypesetter_spill_tab_notation,
                                   mdtypesetter_spill_tuning,
                                   mdtypesetter_spill_song);
}
