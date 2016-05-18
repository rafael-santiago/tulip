#include <dsl/compiler/verifiers/slidedown.h>
#include <dsl/compiler/compiler.h>
#include <dsl/utils.h>
#include <base/ctx.h>

int slidedown_sep_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }
    if (get_cmd_code_from_cmd_tag(buf) != kTlpSlideDown) {
        tlperr_s(error_message, "The slide down separator was expected.");
        return 0;
    }
    (*song) = add_note_to_tulip_single_note_ctx((*song), get_used_techniques() | kTlpSlideDown, NULL);
    (*next) = buf + 1;
    return 1;
}
