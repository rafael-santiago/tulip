/*
 *                           Copyright (C) 2005-2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/html/htmlinkspill.h>
#include <processor/typesetters/html/htmltabviewer_code.h>
#include <processor/typesetters/txt/txt.h>
#include <processor/typesetters/meta.h>
#include <processor/typesetters/svg/svg.h>
#include <processor/typesetters/typeprefs.h>
#include <processor/oututils.h>
#include <processor/settings.h>
#include <encoding/inline_svg_into_html.h>
#include <base/memory.h>
#include <usrland/cmdlineoptions.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define HTMLTYPESETTER_TABVIEW_MAX_SVG_OUTPUT_STREAM_SIZE       (20 * (0x100000)) // INFO(Rafael): 20MB

static void htmltypesetter_spill_tab_notation(FILE *fp, const tulip_single_note_ctx *song);

static void htmltypesetter_spill_transcribers_name(FILE *fp, const char *name);

static void htmltypesetter_spill_song_title(FILE *fp, const char *title);

static void htmltypesetter_spill_tuning(FILE *fp);

static void htmltypesetter_spill_song(FILE *fp, const tulip_single_note_ctx *song);

static int htmltypesetter_inkspill_htmltabviewer(const char *filepath);

int htmltypesetter_inkspill(const char *filepath, const tulip_single_note_ctx *song) {
    if (get_bool_option("svg", 0)) {
        return htmltypesetter_inkspill_htmltabviewer(filepath);
    }

    return metatypesetter_inkspill(filepath, song, " <BR>\n",
                                   htmltypesetter_spill_song_title,
                                   htmltypesetter_spill_transcribers_name,
                                   htmltypesetter_spill_tab_notation,
                                   htmltypesetter_spill_tuning,
                                   htmltypesetter_spill_song);
}

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

static void htmltypesetter_spill_tuning(FILE *fp) {
    struct typesetter_curr_settings cset = typesetter_settings();
    ssize_t s = 0;
    size_t d = 0;
    char **tuning = NULL;

    if ((cset.prefs & kTlpPrefsShowTuning) && !(cset.prefs & kTlpPrefsAddTuningToTheFretboard)) {
        tuning = get_processor_setting("tuning", &d);

        fprintf(fp, " <B>Tuning [%d-1]</B>: ", d);

        for (s = d - 1; s >= 0; s--) {
            fprintf(fp, "%c", tuning[s][0]);

            if (tuning[s][1] != 0 && tuning[s][1] != ' ') {
                fprintf(fp, "%c", tuning[s][1]);
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

static int htmltypesetter_inkspill_htmltabviewer(const char *filepath) {
    // INFO(Rafael): We already know that tulip script has no errors.
    char cmdline[4096], buf[32];
    FILE *svg_typesetter_stream, *fp;
    char *output, *svg, *svg_end, *img_tag, *output_end;
    size_t svg_nr, s;
    ssize_t output_size;
    const char *tulip_script_path = get_option("tlp", NULL);
    const char *option;

    if (tulip_script_path == NULL) {
        // WARN(Rafael): It should never happen in normal conditions.
        fprintf(stderr, "processor ERROR: Null tlp script path.\n");
        return 0;
    }

    if ((fp = fopen(filepath, "wb")) == NULL) {
        fprintf(stderr, "processor ERROR: Unable to write to '%s'\n", filepath);
        return 0;
    }

    // INFO(Rafael): Getting the SVG stream.

    snprintf(cmdline, sizeof(cmdline) - 1, "%s --tlp=%s --out=stdout.svg", get_binary_path(), tulip_script_path);

    // INFO(Rafael): Forwarding relevant SVG typesetter specific options if necessary.

    if ((option = get_option("paper", NULL)) != NULL) {
        strncat(cmdline, " --paper=", sizeof(cmdline) - 1);
        strncat(cmdline, option, sizeof(cmdline) - 1);
    }

    if ((option = get_option("paper-width", NULL)) != NULL) {
        strncat(cmdline, " --paper-width=", sizeof(cmdline) - 1);
        strncat(cmdline, option, sizeof(cmdline) - 1);
    }

    if ((option = get_option("paper-height", NULL)) != NULL) {
        strncat(cmdline, " --paper-height=", sizeof(cmdline) - 1);
        strncat(cmdline, option, sizeof(cmdline) - 1);
    }

    if ((option = get_option("svg-encoding", NULL)) != NULL) {
        strncat(cmdline, "--svg-encoding=", sizeof(cmdline) - 1);
        strncat(cmdline, option, sizeof(cmdline) - 1);
    }

    if ((option = get_option("fretboard-size", NULL)) != NULL) {
        strncat(cmdline, " --fretboard-size=", sizeof(cmdline) - 1);
        strncat(cmdline, option, sizeof(cmdline) - 1);
    }

    // INFO(Rafael): Getting SVG TAB.

    if ((svg_typesetter_stream = popen(cmdline, "r")) == NULL) {
        fprintf(stderr, "processor ERROR: Unable to get the SVG output stream.\n");
        return 0;
    }

    output_size = HTMLTYPESETTER_TABVIEW_MAX_SVG_OUTPUT_STREAM_SIZE;
    output = (char *)getseg(output_size + 1);
    memset(output, 0, output_size + 1);

    while (output_size > 0 && !feof(svg_typesetter_stream)) {
        memset(buf, 0, sizeof(buf));
        fread(buf, 1, sizeof(buf) - 1, svg_typesetter_stream);
        strncat(output, buf, output_size);
        output_size -= sizeof(buf);
    }

    output_size = strlen(output);

    fclose(svg_typesetter_stream);

    // INFO(Rafael): Now is only about parsing and inlining the TAB pages in svg into the HTML document.

    fprintf(fp, HTMLTYPESETTER_TAB_VIEWER_CODE_BEGIN);

    svg_end = output;
    output_end = output + output_size;

    svg_nr = 0;

    while ((svg_end = strstr(svg_end, "</svg>")) != NULL && svg_end < output_end) {
        svg_nr++;
        svg_end += 6;
    }

    svg = output;
    s = 1;

    while ((svg_end = strstr(svg, "</svg>")) != NULL && svg_end < output_end) {
        svg_end += 6;
        if ((img_tag = inline_svg_into_html(svg, svg_end - svg, NULL)) != NULL) {
            fprintf(fp, "<DIV CLASS=\"tabReader fade\">\n"
                        "<CENTER>\n"
                        "%s\n"
                        "</CENTER>\n"
                        "<DIV CLASS=\"page-number\">%d / %d"
                        "</DIV>\n", img_tag, s, svg_nr);
            while (svg_end < output_end && *svg_end == '\n') {
                svg_end++;
            }
            svg = svg_end;
            s++;
            free(img_tag);
        }
    }

    free(output);

    fprintf(fp, HTMLTYPESETTER_TAB_VIEWER_CODE_MIDDLE);

    for (s = 1; s <= svg_nr; s++) {
        fprintf(fp, "<SPAN CLASS=\"dot\" onclick=\"currentPage(%d)\"></SPAN>\n", s);
    }

    fprintf(fp, HTMLTYPESETTER_TAB_VIEWER_CODE_END);

    fclose(fp);

    return 1;
}
