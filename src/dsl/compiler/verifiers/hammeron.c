#include <dsl/compiler/verifiers/hammeron.h>
#include <dsl/compiler/compiler.h>
#include <dsl/utils.h>
#include <base/ctx.h>

int hammeron_sep_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }
    if (get_cmd_code_from_cmd_tag(buf) != kTlpHammerOn) {
        tlperr_s(error_message, "The hammer-on separator was expected.");
        return 0;
    }
    (*song) = add_note_to_tulip_single_note_ctx((*song), get_used_techniques() | kTlpHammerOn, NULL);
    (*next) = buf + 1;
    return 1;
}
