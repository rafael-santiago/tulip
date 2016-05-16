#include <dsl/utils.h>
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
    tulip_register_code_tag(kTlpBeat, ".beat", 1),
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
    tulip_register_code_tag(kTlpReleaseBend, "r", 0)
};

static const size_t g_tlp_tag_map_nr = sizeof(g_tlp_tag_map) / sizeof(g_tlp_tag_map[0]);

static ssize_t get_tlp_tag_index(const char *buf);

int is_single_note(const char *buf) {
    const char *bp = buf;
    const char *strings = "654321";
    size_t offset = 0; //  WARN(Santiago): Nasty trick to support bass tabs, 7-string guitars in the future.
    char string[2] = "";
    if (bp == NULL) {
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
    while (*bp != 0) {
        if (!isdigit(*bp)) {
            return 1;
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
        if (strcmp(g_tlp_tag_map[t].tag, buf) == 0) {
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
