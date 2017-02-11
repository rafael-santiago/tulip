/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/md/mdinkspill.h>
#include <processor/typesetters/txt/txt.h>
#include <processor/typesetters/txt/txtctx.h>
#include <processor/typesetters/typeprefs.h>
#include <processor/settings.h>
#include <processor/oututils.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void mdtypesetter_spill_song_title(FILE *fp, const char *title);

static void mdtypesetter_spill_transcribers_name(FILE *fp, const char *name);

static void mdtypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song);

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
    int d = 0, s = 0;
    char **tunning = NULL;

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

    if (cset.prefs & kTlpPrefsShowTunning) {
        tunning = get_processor_setting("tunning", &d);

        fprintf(fp, "**Tunning [%d-1]**: ", d);

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

static void mdtypesetter_spill_song(FILE *fp, const tulip_single_note_ctx *song) {
    char *tempfile = tlptemp();
    FILE *tp = NULL;
    char *data = NULL;
    long dsize = 0;
    tulip_single_note_ctx *sp, *song_tag = NULL, *transcriber_tag = NULL;

    if (tempfile == NULL) {
        return;
    }

    // INFO(Rafael): The song title/transcriber's name are useless at this point.
    for (sp = (tulip_single_note_ctx *)song; sp != NULL && (song_tag == NULL || transcriber_tag == NULL); sp = sp->next) {
        if (sp->techniques == kTlpSong) {
            song_tag = sp;
        } else if (sp->techniques == kTlpTranscriber) {
            transcriber_tag = sp;
        }
    }

    if (song_tag != NULL) {
        song_tag->techniques = kTlpNone;
    }

    if (transcriber_tag != NULL) {
        transcriber_tag->techniques = kTlpNone;
    }

    if (txt_typesetter(song, tempfile) != 0) {
        goto ___mdtypesetter_spill_song_epilogue;
    }

    if (song_tag != NULL) {
        song_tag->techniques = kTlpSong;
    }

    if (transcriber_tag != NULL) {
        transcriber_tag->techniques = kTlpTranscriber;
    }

    if ((tp = fopen(tempfile, "r")) == NULL) {
        goto ___mdtypesetter_spill_song_epilogue;
    }

    fseek(tp, 0L, SEEK_END);
    dsize = ftell(tp);
    fseek(tp, 0L, SEEK_SET);

    data = (char *) malloc(dsize + 1);
    memset(data, 0, dsize + 1);

    if (data == NULL) {
        goto ___mdtypesetter_spill_song_epilogue;
    }

    fread(data, 1, dsize, tp);

    fprintf(fp, "```\n"
                "%s\n"
                "```\n", data);

    free(data);

    fclose(tp);

___mdtypesetter_spill_song_epilogue:
    remove(tempfile);
}

int mdtypesetter_inkspill(const char *filepath, const tulip_single_note_ctx *song) {
    FILE *fp = fopen(filepath, "w");
    int has_error = 0;
    txttypesetter_tablature_ctx *txttab = NULL;

    if (fp == NULL) {
        has_error = 1;
        goto ___mdtypesetter_inkspill_epilogue;
    }

    if ((txttab = txt_typesetter_gettab(song)) == NULL) {
        has_error = 1;
        goto ___mdtypesetter_inkspill_epilogue;
    }

    mdtypesetter_spill_song_title(fp, txttab->song);

    mdtypesetter_spill_transcribers_name(fp, txttab->transcriber);

    if (txttab->song != NULL || txttab->transcriber != NULL) {
        fprintf(fp, "\n");
    }

    free_txttypesetter_tablature_ctx(txttab);

    mdtypesetter_spill_tab_notation(fp, song);

    mdtypesetter_spill_song(fp, song);

___mdtypesetter_inkspill_epilogue:

    return has_error;
}
