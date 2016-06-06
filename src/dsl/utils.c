/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/utils.h>
#include <base/memory.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct tulip_command_table_ctx {
    const char *tag;
    tulip_command_t code;
    unsigned char can_sustain;
};

#define tulip_register_code_tag(c, t, s) { t, c, s }

static struct tulip_command_table_ctx g_tlp_tag_map[] = {
    tulip_register_code_tag(kTlpMute, ".mute", 1),
    tulip_register_code_tag(kTlpLetRing, ".letring", 1),
    tulip_register_code_tag(kTlpChord, ".chord", 1),
    tulip_register_code_tag(kTlpStrum, ".strum", 1),
    tulip_register_code_tag(kTlpTremoloPicking, ".tremolopicking", 1),
    tulip_register_code_tag(kTlpVibrato, "~", 0),
    tulip_register_code_tag(kTlpSlideDown, "/", 0),
    tulip_register_code_tag(kTlpSlideUp, "\\", 0),
    tulip_register_code_tag(kTlpHammerOn, "h", 0),
    tulip_register_code_tag(kTlpPullOff, "p", 0),
    tulip_register_code_tag(kTlpVibratoWBar, ".vibratowbar", 1),
    tulip_register_code_tag(kTlpTunning, ".tunning", 0),
    tulip_register_code_tag(kTlpLiteral, ".literal", 0),
    tulip_register_code_tag(kTlpLiteral, ".blah", 0),
    tulip_register_code_tag(kTlpLiteral, ".quote", 0),
    tulip_register_code_tag(kTlpNoteSep, "-", 0),
    tulip_register_code_tag(kTlpSepBar, "|", 0),
    tulip_register_code_tag(kTlpSavePoint, ";", 0),
    tulip_register_code_tag(kTlpBend, "b", 0),
    tulip_register_code_tag(kTlpReleaseBend, "r", 0),
    tulip_register_code_tag(kTlpTapping, "T", 0),
    tulip_register_code_tag(kTlpNaturalHarmonic, "*", 0),
    tulip_register_code_tag(kTlpArtificialHarmonic, "v", 0),
    tulip_register_code_tag(kTlpBlockEnd, "}", 0)
};

static const size_t g_tlp_tag_map_nr = sizeof(g_tlp_tag_map) / sizeof(g_tlp_tag_map[0]);

static ssize_t get_tlp_tag_index(const char *buf);

size_t tlp_cmd_code_to_plain_index(const tulip_command_t code) {
    int carry = 0;
    int index = 0;
    if ((code & 0x0000000f) != 0) {
        carry = 0;
    } else if ((code & 0x000000f0) != 0) {
        carry = 4;
    } else if ((code & 0x00000f00) != 0) {
        carry = 8;
    } else if ((code & 0x0000f000) != 0) {
        carry = 12;
    } else if ((code & 0x000f0000) != 0) {
        carry = 16;
    } else if ((code & 0x00f00000) != 0) {
        carry = 20;
    } else if ((code & 0x0f000000) != 0) {
        carry = 24;
    } else if ((code & 0xf0000000) != 0) {
        carry = 28;
    }
    switch (code >> carry) {
        case 0x01:
            index = 0;
            break;

        case 0x02:
            index = 1;
            break;

        case 0x04:
            index = 2;
            break;

        case 0x08:
            index = 3;
            break;
    }
    return (index + carry);
}

int is_single_note(const char *buf) {
    const char *bp = buf;
    const char *strings = "654321";
    size_t offset = 0; //  WARN(Santiago): Nasty trick to support bass tabs, 7-string guitars in the future.
    char string[2] = "";
    int fret = 0;
    if (bp == NULL) {
        return 0;
    }
    if (strlen(buf) < 2) {
        return 0;
    }
    string[0] = *bp;
    string[1] = 0;
    if (strstr(strings + offset, string) == NULL) {
        return 0;
    }
    bp++;
    if (*bp == 0) {
        return 0;
    }
    if ((*(bp) == 'X' || *(bp) == ':') || *(bp) == '?') {
        if (*bp == 'X' && (*(bp + 1) == 0 || is_blank(*(bp + 1)))) {
            return 1;
        }
        if (*bp == '?' && (*(bp + 1) == 0 || is_blank(*(bp + 1)))) {
            return 1;
        }
        bp++;
        if (!is_note_sep(*bp)) {
            return 0;
        }
        return 1;
    }
    while (*bp != 0) {
        if (!isdigit(*bp)) {
            if (fret > 0 && (is_note_sep(*bp) || is_technique_block_end(*bp) || is_blank(*bp))) {
                return 1;
            }
            return 0;
        } else {
            fret++;
        }
        bp++;
    }
    return 1;
}

int is_sustained_technique(const char *buf) {
    ssize_t t = get_tlp_tag_index(buf);
    return (t > -1 && t < g_tlp_tag_map_nr) ? g_tlp_tag_map[t].can_sustain : 0;
}

static ssize_t get_tlp_tag_index(const char *buf) {
    size_t t;

    if (buf == NULL) {
        return -1;
    }

    for (t = 0; t < g_tlp_tag_map_nr; t++) {
        if (strstr(buf, g_tlp_tag_map[t].tag) == buf) {
            return t;
        }
    }

    return -1;
}

tulip_command_t get_cmd_code_from_cmd_tag(const char *buf) {
    ssize_t t = get_tlp_tag_index(buf);

    if (t > -1 && t < g_tlp_tag_map_nr) {
        return g_tlp_tag_map[t].code;
    }

    if (is_single_note(buf)) {
        return kTlpSingleNote;
    }

    return kTlpNone;
}

int is_valid_note_cipher(const char *cipher) {
    const char *ciphers[] = {
        "C" , "D" , "E" , "F" , "G" , "A" , "B" , "C#", "D#",
        "F#", "G#", "A#", "Db", "Eb", "Gb", "Ab", "Bb", "c" ,
        "d" , "e" , "f" , "g" , "a" , "b" , "c#", "d#", "f#",
        "g#", "db", "eb", "gb", "ab", "bb"
    };
    const size_t cipher_nr = sizeof(ciphers) / sizeof(ciphers[0]);
    size_t c;
    for (c = 0; c < cipher_nr; c++) {
        if (strcmp(cipher, ciphers[c]) == 0) {
            return 1;
        }
    }
    return 0;
}

tulip_command_t *demux_tlp_commands(const tulip_command_t commands, size_t *array_size) {
    tulip_command_t *dmux = NULL;
    tulip_command_t temp = commands;
    size_t b = 0;
    size_t a = 0;
    if (array_size == NULL || commands == kTlpNone) {
        return kTlpNone;
    }
    (*array_size) = 0;
    for (b = 0; b < sizeof(tulip_command_t) * 8; b++) {
        (*array_size) += (temp & 1);
        temp = temp >> 1;
    }
    dmux = (tulip_command_t *) getseg(sizeof(tulip_command_t *) + sizeof(tulip_command_t) * *array_size);
    //  TODO(Santiago): Change the order of tulip_command_t (kTlpBlockEnd) must be the lower value in this enum.
    a = *array_size - 1;
    for (b = 0; b < sizeof(tulip_command_t) * 8; b++) {
        temp = 1 << b;
        if (commands & temp) {
            dmux[a--] = temp;
        }
    }
    return dmux;
}
