/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
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
#include <dsl/compiler/verifiers/tunning.h>
#include <dsl/compiler/verifiers/letring.h>
#include <dsl/compiler/verifiers/mute.h>
#include <dsl/compiler/verifiers/beat.h>
#include <dsl/compiler/verifiers/tremolopicking.h>
#include <dsl/compiler/verifiers/vibratowbar.h>
#include <dsl/compiler/verifiers/tapping.h>
#include <dsl/compiler/verifiers/naturalharmonic.h>
#include <dsl/compiler/verifiers/artificialharmonic.h>
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
    tlp_compiler_register_cmd_verifier(kTlpMute, mute_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpLetRing, letring_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpChord, chord_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpBeat, beat_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpTremoloPicking, tremolopicking_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpVibrato, vibrato_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSlideDown, slidedown_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSlideUp, slideup_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpHammerOn, hammeron_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpPullOff, pulloff_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpVibratoWBar, vibratowbar_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpTunning, tunning_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpLiteral, literal_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSingleNote, singlenote_verifier),
    tlp_compiler_register_cmd_verifier(kTlpNoteSep, notesep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSepBar, sepbar_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSavePoint, savepoint_verifier),
    tlp_compiler_register_cmd_verifier(kTlpBend, bend_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpReleaseBend, releasebend_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpBlockEnd, blockend_verifier),
    tlp_compiler_register_cmd_verifier(kTlpTapping, tapping_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpNaturalHarmonic, naturalharmonic_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpArtificialHarmonic, artificialharmonic_sep_verifier)
};

size_t g_tlp_cmd_verifiers_nr = sizeof(g_tlp_cmd_verifiers) / sizeof(g_tlp_cmd_verifiers[0]);

static int verify_curr_command(const tulip_command_t cmd, const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

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
    strcat(buf, "\n");
    va_end(args);
}

int compile_tulip_codebuf(const char *codebuf, char *message_buf, tulip_single_note_ctx **song) {
    const char *next = NULL;
    const char *cp = codebuf;
    const char *cp_end = NULL;
    static int callstack_level = 0;
    int cltmp = callstack_level++;
    tulip_single_note_ctx *sp = NULL;
    tulip_command_t curr_command = kTlpNone;
    int compilation_status = 1;
    if (cp == NULL) {
        return 0;
    }
    cp_end = cp + strlen(cp);
    if (cltmp == 0) {
        set_curr_code_line_number(1);
    }
    if (message_buf != 0) {
        *message_buf = 0;
    }
    sp = *song;
    while (cp != cp_end && compilation_status != 0) {
        cp = get_next_tlp_command(cp);
//        printf("*** %s\n", cp);
        if (cp == cp_end) {
            continue;
        }
        if ((curr_command = get_cmd_code_from_cmd_tag(cp)) == kTlpNone) {
            tlperr_s(message_buf, "Unknown sequence: %s", cp);
            compilation_status = 0;
        } else {
         compilation_status = verify_curr_command(curr_command, cp, message_buf, song, &cp);
        }
        if (compilation_status == 0) {
            if ((*song) != NULL) {
                free_tulip_single_note_ctx((*song));
                (*song) = NULL;
            }
        }
    }
    if (cltmp == 0) {
        callstack_level = 0;
        if ((*song) != NULL) { //  INFO(Santiago): A.k.a "compilation_status == 1".
            if (!is_empty_technique_stack_ctx(g_techniques)) {
                tlperr_s(message_buf, "The code has some unterminated tag, please check and try again.");
                free_tulip_single_note_ctx((*song));
                (*song) = NULL;
                free_technique_stack_ctx(g_techniques);
                g_techniques = NULL;
                compilation_status = 0;
            }
            if (compilation_status) {
                sprintf(message_buf, "tulip INFO: compilation success.\n");
            }
        }
    }
    return compilation_status;
}

static int verify_curr_command(const tulip_command_t cmd, const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    return g_tlp_cmd_verifiers[tlp_cmd_code_to_plain_index(cmd)].verifier(buf, error_message, song, next);
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
