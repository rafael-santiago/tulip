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
#include <dsl/compiler/verifiers/strum.h>
#include <dsl/compiler/verifiers/tremolopicking.h>
#include <dsl/compiler/verifiers/vibratowbar.h>
#include <dsl/compiler/verifiers/tapping.h>
#include <dsl/compiler/verifiers/naturalharmonic.h>
#include <dsl/compiler/verifiers/artificialharmonic.h>
#include <dsl/compiler/verifiers/times.h>
#include <dsl/compiler/verifiers/oncemore.h>
#include <dsl/compiler/verifiers/part.h>
#include <dsl/compiler/verifiers/repeat.h>
#include <dsl/compiler/verifiers/song.h>
#include <dsl/compiler/verifiers/transcriber.h>
#include <dsl/compiler/verifiers/trill.h>
#include <dsl/parser/parser.h>
#include <dsl/utils.h>
#include <base/ctx.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

tulip_technique_stack_ctx *g_techniques = NULL;

tulip_part_ctx *g_parts = NULL;

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

#define verify_curr_command(c, b, e, s, n) ( g_tlp_cmd_verifiers[tlp_cmd_code_to_plain_index((c))].verifier((b), (e), (s), (n)) )

#define tlp_compiler_register_cmd_verifier(c, v) { v } //  INFO(Santiago): "c" is just for making the things clearer.

static struct tlp_command_verifiers_ctx g_tlp_cmd_verifiers[] = {
    tlp_compiler_register_cmd_verifier(kTlpSavePoint, savepoint_verifier),
    tlp_compiler_register_cmd_verifier(kTlpNoteSep, notesep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpBlockEnd, blockend_verifier),
    tlp_compiler_register_cmd_verifier(kTlpMute, mute_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpLetRing, letring_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpChord, chord_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpStrum, strum_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpTremoloPicking, tremolopicking_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpVibratoWBar, vibratowbar_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpTrill, trill_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpTimes, times_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpVibrato, vibrato_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSlideDown, slidedown_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSlideUp, slideup_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpHammerOn, hammeron_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpPullOff, pulloff_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpTunning, tunning_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpLiteral, literal_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSingleNote, singlenote_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSepBar, sepbar_verifier),
    tlp_compiler_register_cmd_verifier(kTlpBend, bend_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpReleaseBend, releasebend_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpTapping, tapping_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpNaturalHarmonic, naturalharmonic_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpArtificialHarmonic, artificialharmonic_sep_verifier),
    tlp_compiler_register_cmd_verifier(kTlpSong, song_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpTranscriber, transcriber_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpOnceMore, oncemore_verifier),
    tlp_compiler_register_cmd_verifier(kTlpPart, part_tag_verifier),
    tlp_compiler_register_cmd_verifier(kTlpRepeat, repeat_tag_verifier)
};

size_t g_tlp_cmd_verifiers_nr = sizeof(g_tlp_cmd_verifiers) / sizeof(g_tlp_cmd_verifiers[0]);

static char *get_tag_from_compiler_stack();

static void free_parts_listing();

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

static char *get_tag_from_compiler_stack() {
    static char *tags[] = {
        NULL,
        NULL,
        NULL,
        ".mute",
        ".letring",
        ".chord",
        ".beat",
        ".tremolopicking",
        ".vibratowbar",
        ".trill"
    };
    const size_t tags_nr = sizeof(tags) / sizeof(tags[0]);
    return &tags[tlp_cmd_code_to_plain_index(top_of_technique_stack_ctx(g_techniques)) % tags_nr][0];
}

int compile_tulip_codebuf(const char *codebuf, char *message_buf, tulip_single_note_ctx **song, const char **next_codebuf) {
    const char *next = NULL;
    const char *cp = codebuf;
    const char *cp_end = NULL;
    static int callstack_level = 0;
    int cltmp = callstack_level++;
    tulip_single_note_ctx *sp = NULL, *p;
    tulip_command_t curr_command = kTlpNone;
    int compilation_status = 1;
    char command_chunk[20] = "";

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

        if (cp == cp_end) {
            continue;
        }

        if ((curr_command = get_cmd_code_from_cmd_tag(cp)) == kTlpNone) {
            memset(command_chunk, 0, sizeof(command_chunk));
            memcpy(command_chunk, cp, 19);
            tlperr_s(message_buf, "Unknown sequence: %s", command_chunk);
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

                tlperr_s(message_buf, "The code has some unterminated tag, please check and try again. "
                                      "Tip: pay attention on the \"%s\" tag occurrences or on the tags that are going inside "
                                      "them.", get_tag_from_compiler_stack());
                free_tulip_single_note_ctx((*song));
                (*song) = NULL;
                free_technique_stack_ctx(g_techniques);
                g_techniques = NULL;
                compilation_status = 0;

            }

            if (compilation_status) {
                if (message_buf != NULL) {
                    sprintf(message_buf, "tulip INFO: compilation success.\n");
                }
            }

        }
        free_parts_listing();
    } else if (next_codebuf != NULL) {
        *next_codebuf = cp;
    }

    return compilation_status;
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

tulip_part_ctx *get_parts_listing() {
    return g_parts;
}

static void free_parts_listing() {
    free_tulip_part_ctx(g_parts);
    g_parts = NULL;
}

void set_parts_listing(tulip_part_ctx *parts) {
    g_parts = parts;
}

int has_some_sustained_technique() {
    return (g_techniques != NULL);
}
