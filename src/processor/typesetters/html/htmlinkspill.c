/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/html/htmlinkspill.h>
#include <processor/typesetters/txt/txt.h>
#include <processor/typesetters/meta.h>
#include <processor/typesetters/typeprefs.h>
#include <processor/oututils.h>
#include <processor/settings.h>
#include <stdlib.h>
#include <stdio.h>

static void htmltypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song);

static void htmltypesetter_spill_transcribers_name(FILE *fp, const char *name);

static void htmltypesetter_spill_song_title(FILE *fp, const char *title);

static void htmltypesetter_spill_tunning(FILE *fp);

static void htmltypesetter_spill_song(FILE *fp, const tulip_single_note_ctx *song);

static void htmltypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song) {
    struct typesetter_curr_settings cset = typesetter_settings();
    int has_muffled = 0, has_anyfret = 0;
    tulip_command_t used_techniques[31];
    size_t used_techniques_nr = 0, u = 0;

    if ((cset.prefs & kTlpPrefsIncludeTabNotation) == 0 || song == NULL) {
        return;
    }

    get_all_used_techniques(song, used_techniques, &used_techniques_nr, &has_muffled, &has_anyfret);

    if (used_techniques > 0 || has_muffled || has_anyfret) {
        fprintf(fp, " <TABLE BORDER = 0>\n"
                    "  <TR><TD><B>Technique</B></TD><TD><B>Description</B></TD></TR>\n");

        for (u = 0; u < used_techniques_nr; u++) {
            fprintf(fp, "  <TR><TD>%s</TD><TD>%s</TD></TR>\n", get_technique_label(used_techniques[u]),
                                                             get_technique_notation_label(used_techniques[u]));
        }

        if (has_muffled) {
            fprintf(fp, "  <TR><TD>X</TD><TD>Muffled note</TD></TR>\n");
        }

        if (has_anyfret) {
            fprintf(fp, "  <TR><TD>?</TD><TD>From any fret</TD></TR>\n");
        }

        fprintf(fp, " </TABLE>\n"
                    " <BR>\n");
    }
}

static void htmltypesetter_spill_transcribers_name(FILE *fp, const char *name) {
    if (name == NULL) {
        return;
    }
    fprintf(fp, " <SMALL><I>transcribed by %s</I></SMALL><BR><BR>\n", name);
}

static void htmltypesetter_spill_song_title(FILE *fp, const char *title) {
    fprintf(fp, "<HTML>\n"
                " <HEAD>\n"
                "  <META CHARSET=\"UTF-8\">\n"
                " </HEAD>\n");

    if (title == NULL) {
        return;
    }

    fprintf(fp, " <H1>%s</H1>\n", title);
}

static void htmltypesetter_spill_tunning(FILE *fp) {
    struct typesetter_curr_settings cset = typesetter_settings();
    ssize_t s = 0;
    size_t d = 0;
    char **tunning = NULL;

    if ((cset.prefs & kTlpPrefsShowTunning) && !(cset.prefs & kTlpPrefsAddTunningToTheFretboard)) {
        tunning = get_processor_setting("tunning", &d);

        fprintf(fp, " <B>Tunning [%d-1]</B>: ", d);

        for (s = d - 1; s >= 0; s--) {
            fprintf(fp, "%c", tunning[s][0]);

            if (tunning[s][1] != 0 && tunning[s][1] != ' ') {
                fprintf(fp, "%c", tunning[s][1]);
            }

            if (s != 0) {
                fprintf(fp, ", ");
            } else {
                fprintf(fp, "<BR>");
            }
        }

        fprintf(fp, "<BR>\n");
    }
}

static void htmltypesetter_spill_song(FILE *fp, const tulip_single_note_ctx *song) {
    char *data = NULL;
    data = typesetter_raw_output(song, NULL, txt_typesetter);

    if (data != NULL) {
        fprintf(fp, " <PRE>\n%s</PRE>\n", data);
        free(data);
    }

    fprintf(fp, "</HTML>");
}

int htmltypesetter_inkspill(const char *filepath, const tulip_single_note_ctx *song) {
    return metatypesetter_inkspill(filepath, song, " <BR>\n",
                                   htmltypesetter_spill_song_title,
                                   htmltypesetter_spill_transcribers_name,
                                   htmltypesetter_spill_tab_notation,
                                   htmltypesetter_spill_tunning,
                                   htmltypesetter_spill_song);
}
