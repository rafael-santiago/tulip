#include <dsl/compiler/compiler.h>
#include <dsl/compiler/verifiers/chord.h>
#include <dsl/compiler/verifiers/literal.h>
#include <dsl/compiler/verifiers/vibrato.h>
#include <dsl/compiler/verifiers/slidedown.h>
#include <dsl/compiler/verifiers/slideup.h>
#include <dsl/compiler/verifiers/hammeron.h>
#include <dsl/compiler/verifiers/pulloff.h>
#include <dsl/compiler/verifiers/notesep.h>
#include <dsl/compiler/verifiers/sepbar.h>
#include <dsl/compiler/verifiers/savepoint.h>
#include <dsl/compiler/verifiers/bend.h>
#include <dsl/compiler/verifiers/releasebend.h>
#include <dsl/compiler/verifiers/singlenote.h>
#include <dsl/compiler/verifiers/blockend.h>
#include <dsl/parser/parser.h>
#include <dsl/utils.h>
#include <base/ctx.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

tulip_technique_stack_ctx *g_techniques = NULL;

//  INFO(Santiago): This must be the general standard behavior of a verifier...
//
//            - Verify the current code chunk;
//
//            - If it is fine the code chunk must be loaded into the passed (tulip_single_note_ctx *) set [or as the musicians
//              like to call: "song"] and (**next) must point to the next unverified chunk. The return is always 1;
//
//            - Otherwise if it has some error we need to return 0 and let the caller handle this;
//
//  It has been done in order to save one compiler pass.

struct tlp_command_verifiers_ctx {
    int (*verifier)(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);
};

#define tlp_compiler_register_cmd_verifier(c, v) { v } //  INFO(Santiago): "c" is just for making the things clearer.

static struct tlp_command_verifiers_ctx g_tlp_cmd_verifiers[] = {
    tlp_compiler_register_cmd_verifier(kTlpMute, NULL),
    tlp_compiler_register_cmd_verifier(kTlpLetRing, NULL),
    tlp_compiler_register_cmd_verifier(kTlpChord, chord_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpBeat, NULL),
    tlp_compiler_register_cmd_verifier(kTlpTremoloPicking, NULL),
    tlp_compiler_register_cmd_verifier(kTlpVibrato, vibrato_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSlideDown, slidedown_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSlideUp, slideup_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpHammerOn, hammeron_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpPullOff, pulloff_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpVibratoWBar, NULL),
    tlp_compiler_register_cmd_verifier(kTlpTunning, NULL),
    tlp_compiler_register_cmd_verifier(kTlpLiteral, literal_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSingleNote, singlenote_verifier),
    tlp_compiler_register_cmd_verifier(kTlpNoteSep, notesep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSepBar, sepbar_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSavePoint, savepoint_verifier),
    tlp_compiler_register_cmd_verifier(kTlpBend, bend_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpReleaseBend, releasebend_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpBlockEnd, blockend_verifier)
};

size_t g_tlp_cmd_verifiers_nr = sizeof(g_tlp_cmd_verifiers) / sizeof(g_tlp_cmd_verifiers[0]);

void tlperr_s(char *buf, const char *error_message, ...) {
    char *bp = buf;
    const char *ep = error_message;
    const char *ep_end = NULL;
    char temp[255] = "";
    char arg_c = 0, *arg_s = NULL;
    int arg_i = 0;
    va_list args;
    if (buf == NULL || error_message == NULL) {
        return;
    }
    sprintf(buf, "tulip ERROR: LINE: %d: ", get_curr_code_line_number());
    ep_end = ep + strlen(ep);
    va_start(args, error_message);
    while (ep != ep_end) {
        if (*ep == '%') {
            ep++;
            switch (*ep) {
                case 's':
                    arg_s = va_arg(args, char *);
                    sprintf(temp, "%s", arg_s);
                    break;

                case 'c':
                    arg_c = (char) va_arg(args, int);
                    sprintf(temp, "%c", arg_c);
                    break;

                case 'd':
                    arg_i = va_arg(args, int);
                    sprintf(temp, "%d", arg_i);
                    break;
            }
        } else {
            temp[0] = *ep;
            temp[1] = 0;
        }
        strcat(buf, temp);
        ep++;
    }
    va_end(args);
}

tulip_single_note_ctx *compile_tulip_codebuf(const char *codebuf, char *message_buf) {
    const char *next = NULL;
    const char *cp = codebuf;
    const char *cp_end = NULL;
    tulip_single_note_ctx *song = NULL;
    tulip_command_t curr_command = kTlpNone;
    if (cp == NULL) {
        return NULL;
    }
    cp_end = cp + strlen(codebuf);
    set_curr_code_line_number(1);
    while (cp != cp_end) {
        cp = get_next_tlp_command(cp);
        if ((curr_command = get_cmd_code_from_cmd_tag(cp)) == kTlpNone) {
            tlperr_s(message_buf, "Unknown sequence: %c", *cp);
        }
    }
    return song;
}

tulip_command_t get_used_techniques() {
    tulip_command_t techniques = kTlpNone;
    tulip_technique_stack_ctx *tp = NULL;
    if (g_techniques == NULL) {
        return kTlpNone;
    }
    for (tp = g_techniques; tp != NULL; tp = tp->next) {
        techniques |= tp->technique_code;
    }
    return techniques;
}

void push_technique(const tulip_command_t technique) {
    g_techniques = push_technique_to_technique_stack_ctx(g_techniques, technique);
}

void pop_technique() {
    g_techniques = pop_technique_from_technique_stack_ctx(g_techniques);
}
