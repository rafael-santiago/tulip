#include <dsl/compiler/verifiers/singlenote.h>
#include <dsl/compiler/compiler.h>
#include <dsl/utils.h>
#include <base/ctx.h>
#include <string.h>

int singlenote_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    char note[255];
    char *np = NULL, *np_end = NULL;
    const char *bp = NULL;
    const char *bp_end = NULL;
    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }
    if (get_cmd_code_from_cmd_tag(buf) != kTlpSingleNote) {
        tlperr_s(error_message, "A single note was expected but was found: \"%s\"", buf);
        return 0;
    }
    bp = buf;
    bp = bp + strlen(buf);
    np = &note[0];
    np_end = np + sizeof(note);
    while (!is_note_sep(*bp) && bp != bp_end) {
        *np = *bp;
        bp++;
        np++;
    }
    *np = 0;
    (*song) = add_note_to_tulip_single_note_ctx((*song), get_used_techniques() | kTlpSingleNote, note);
    (*next) = bp;
    return 1;
}
