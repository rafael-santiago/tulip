/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/oututils.h>
#include <dsl/utils.h>
#include <stdlib.h>
#include <string.h>

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
