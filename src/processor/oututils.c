/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/oututils.h>
#include <processor/settings.h>
#include <processor/typesetters/typeprefs.h>
#include <dsl/utils.h>
#include <base/memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char *g_tlp_technique_label[] = {
    "",
    "-",
    "",
    "pm",
    "lr",
    "",
    "st",
    "tp",
    "vb",
    "tr",
    "",
    "~",
    "/",
    "\\",
    "h",
    "p",
    "",
    "",
    "",
    "|",
    "b",
    "r",
    "T",
    "*",
    "v",
    "",
    "",
    "",
    "",
    ""
};

static char *g_tlp_technique_notation_label[] = {
    "",
    "",
    "",
    "Palm mute",
    "Let ring",
    "",
    "Strum",
    "Tremolo Picking",
    "Vibrato with bar",
    "Trill",
    "",
    "Vibrato",
    "Slide down",
    "Slide up",
    "Hammer-on",
    "Pull-off",
    "",
    "",
    "",
    "",
    "Bend",
    "Release bend",
    "Tapping",
    "Natural harmonic",
    "Artificial harmonic",
    "",
    "",
    "",
    "",
    ""
};

const size_t g_tlp_technique_label_nr = sizeof(g_tlp_technique_label) / sizeof(g_tlp_technique_label[0]);

char *get_technique_label(const tulip_command_t command) {
    return &g_tlp_technique_label[tlp_cmd_code_to_plain_index(command) % g_tlp_technique_label_nr][0];
}

int single_note_to_tab_fret_nr(const char *single_note) {
    if (single_note == NULL || strlen(single_note) == 1) {
        return 0;
    }
    if (strlen(single_note) > 2) {
        return atoi(&single_note[1]) + 10;
    }
    return atoi(&single_note[1]);
}

char *get_technique_notation_label(const tulip_command_t command) {
    return &g_tlp_technique_notation_label[tlp_cmd_code_to_plain_index(command) % g_tlp_technique_label_nr][0];
}

int is_tab_empty(const txttypesetter_tablature_ctx *tab) {
    size_t s = 0, o = 0;
    int is_empty = 1;
    //  WARN(Santiago): A leading ';' at the end of a tlp script or a automatic break just after
    //                  the last song note can cause this dummy empty tab diagram.
    for (s = 0; s < tab->string_nr && is_empty; s++) {
        for (o = 0; o < tab->fretboard_sz && is_empty; o++) {
            is_empty = (tab->strings[s][o] == '-');
        }
    }

    return is_empty;
}

int tunning_has_half_step_notes(const txttypesetter_tablature_ctx *tab, const char tunning_buffer[6][4], const tulip_prefs_map_t prefs) {
    int has_half_step_notes = 0;
    size_t i = 0;
    char tunning[6][4];

    if (prefs & kTlpPrefsAddTunningToTheFretboard) {
        if (tab != NULL) {
            memcpy(tunning, tab->tunning, sizeof(tunning));
        } else {
            memcpy(tunning, tunning_buffer, sizeof(tunning));
        }
        for (i = 0; i < 6 && !has_half_step_notes; i++) {
            has_half_step_notes = (strlen(tunning[i]) == 2);
        }
    }

    return has_half_step_notes;
}

ssize_t get_fretboard_usage_limit(const txttypesetter_tablature_ctx *tab) {
    size_t s = 0;
    size_t i = 0;
    ssize_t f_limit = -1;

    while (s < tab->string_nr) {
        for (i = strlen(tab->strings[s]) - 1; i > 0 && is_sep(tab->strings[s][i]); i--);

        i++;

        if (!is_sep(tab->strings[s][i - 1]) && !is_sep_bar(tab->strings[s][i - 1])) {
            i++;
        } else if (is_sep_bar(tab->strings[s][i - 1])) {
            i--;
        }

        if ((ssize_t)i > f_limit) {
            f_limit = (ssize_t)i;
        }

        s++;
    }

    return f_limit;
}


void get_all_used_techniques(const tulip_single_note_ctx *song, tulip_command_t used_techniques[31], size_t *used_techniques_nr, int *has_muffled, int *has_anyfret) {
    size_t u = 0, demuxes_nr = 0, d = 0, u_nr = 0;
    const tulip_single_note_ctx *sp = NULL;
    int found = 0;
    tulip_command_t *demuxes = NULL;

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

    for (sp = song; sp != NULL && u_nr < 32; sp = sp->next) {
        demuxes = demux_tlp_commands(sp->techniques, &demuxes_nr);

        for (d = 0; d < demuxes_nr; d++) {
            if (is_relevant_technique(demuxes[d])) {
                found = 0;
                for (u = 0; u < u_nr && !found; u++) {
                    found = (demuxes[d] == used_techniques[u]);
                }
                if (!found) {
                    used_techniques[u] = demuxes[d];
                    u_nr++;
                }
            } else if (sp->buf[0] != 0) {
                if (has_muffled != NULL && !(*has_muffled) && (sp->techniques & kTlpTimes) == 0) {
                    *has_muffled = (strstr(sp->buf, "X") != NULL);
                }
                if (has_anyfret != NULL && !(*has_anyfret)) {
                    *has_anyfret = (strstr(sp->buf, "?") != NULL);
                }
            }
        }

        free(demuxes);
    }

#undef is_relevant_technique

    if (used_techniques_nr != NULL) {
        *used_techniques_nr = u_nr;
    }
}

char *tlptemp(void) {
    static char tempfilename[255];
    int t = 0;
    FILE *fp = NULL;


    do {
        if (fp != NULL) {
            fclose(fp);
        }
        sprintf(tempfilename, ".tlptemp%d", t++);
        fp = fopen(tempfilename, "r");
    } while (fp != NULL && t < 0xffffffff);

    if (fp != NULL) {
        fclose(fp);
        sprintf(tempfilename, ".tlptemp.ahhhhhhh");
    }

    return &tempfilename[0];
}

char *typesetter_raw_output(const tulip_single_note_ctx *song, size_t *osize, int (*typesetter)(const tulip_single_note_ctx *song, const char *tempfile)) {
    char *tempfile = tlptemp();
    struct typesetter_curr_settings cset = typesetter_settings();
    char *data = NULL;
    long dsize = 0;
    tulip_single_note_ctx *sp, *song_tag = NULL, *transcriber_tag = NULL;
    tulip_prefs_map_t oldprefs = cset.prefs;
    FILE *tp = NULL;

    if (tempfile == NULL) {
        return NULL;
    }

    // INFO(Rafael): The song title/transcriber's name are useless at this point.
    for (sp = (tulip_single_note_ctx *)song; sp != NULL && (song_tag == NULL || transcriber_tag == NULL); sp = sp->next) {
        if (sp->techniques == kTlpSong) {
            song_tag = sp;
        } else if (sp->techniques == kTlpTranscriber) {
            transcriber_tag = sp;
        }
    }

    // INFO(Rafael): Nasty tricks for not doing these things twice with txttypesetting and mdtypesetting.

    if (song_tag != NULL) {
        song_tag->techniques = kTlpNone;
    }

    if (transcriber_tag != NULL) {
        transcriber_tag->techniques = kTlpNone;
    }

    cset.prefs = cset.prefs & ~(kTlpPrefsShowTunning | kTlpPrefsIncludeTabNotation);
    set_processor_setting("prefs", &cset.prefs, sizeof(cset.prefs));

    if (typesetter(song, tempfile) != 0) {
        goto ___typesetter_raw_output_epilogue;
    }

    set_processor_setting("prefs", &oldprefs, sizeof(oldprefs));

    if (song_tag != NULL) {
        song_tag->techniques = kTlpSong;
    }

    if (transcriber_tag != NULL) {
        transcriber_tag->techniques = kTlpTranscriber;
    }

    if ((tp = fopen(tempfile, "r")) == NULL) {
        goto ___typesetter_raw_output_epilogue;
    }

    fseek(tp, 0L, SEEK_END);
    dsize = ftell(tp);
    fseek(tp, 0L, SEEK_SET);

    data = (char *) getseg(dsize + 1);
    memset(data, 0, dsize + 1);

    fread(data, 1, dsize, tp);

    if (osize != NULL) {
        *osize = dsize;
    }

___typesetter_raw_output_epilogue:

    remove(tempfile);

    return data;
}
