#include <dsl/compiler/compiler.h>
#include <dsl/parser/parser.h>
#include <dsl/utils.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

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
    int (*verifier)(const char *buf, tulip_single_note_ctx **song, char **next);
};

#define tlp_compiler_register_cmd_verifier(c, v) { v } //  INFO(Santiago): "c" is just for making the things clearer.

static struct tlp_command_verifiers_ctx g_tlp_cmd_verifiers[] = {
    tlp_compiler_register_cmd_verifier(kTlpMute, NULL),
    tlp_compiler_register_cmd_verifier(kTlpLetRing, NULL),
    tlp_compiler_register_cmd_verifier(kTlpChord, NULL),
    tlp_compiler_register_cmd_verifier(kTlpBeat, NULL),
    tlp_compiler_register_cmd_verifier(kTlpTremoloPicking, NULL),
    tlp_compiler_register_cmd_verifier(kTlpVibrato, NULL),
    tlp_compiler_register_cmd_verifier(kTlpSlideDown, NULL),
    tlp_compiler_register_cmd_verifier(kTlpSlideUp, NULL),
    tlp_compiler_register_cmd_verifier(kTlpHammerOn, NULL),
    tlp_compiler_register_cmd_verifier(kTlpPullOff, NULL),
    tlp_compiler_register_cmd_verifier(kTlpVibratoWBar, NULL),
    tlp_compiler_register_cmd_verifier(kTlpTunning, NULL),
    tlp_compiler_register_cmd_verifier(kTlpLiteral, NULL),
    tlp_compiler_register_cmd_verifier(kTlpSingleNote, NULL),
    tlp_compiler_register_cmd_verifier(kTlpNoteSep, NULL),
    tlp_compiler_register_cmd_verifier(kTlpSepBar, NULL),
    tlp_compiler_register_cmd_verifier(kTlpSavePoint, NULL),
    tlp_compiler_register_cmd_verifier(kTlpBend, NULL),
    tlp_compiler_register_cmd_verifier(kTlpReleaseBend, NULL),
    tlp_compiler_register_cmd_verifier(kTlpBlockEnd, NULL)
};

size_t g_tlp_cmd_verifiers_nr = sizeof(g_tlp_cmd_verifiers) / sizeof(g_tlp_cmd_verifiers[0]);

static void tlperr_s(char *buf, const char *error_message, ...);

static void tlperr_s(char *buf, const char *error_message, ...) {
    char *bp = buf;
    const char *ep = error_message;
    const char *ep_end = NULL;
    char temp[255] = "";
    char arg_c = 0, *arg_s = NULL;
    int arg_i = 0;
    va_list args;
    if (buf != NULL && error_message != NULL) {
        sprintf(buf, "tulip ERROR: LINE: %d: ", get_curr_code_line_number());
    }
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
            if (message_buf != NULL) {
                tlperr_s(message_buf, "Unknown sequence: %c", *cp);
            }
        }
    }
    return song;
}

