#include <dsl/compiler/verifiers/part.h>
#include <dsl/compiler/verifiers/oncemore.h>
#include <dsl/compiler/compiler.h>
#include <dsl/utils.h>
#include <dsl/parser/parser.h>
#include <base/ctx.h>
#include <ctype.h>
#include <string.h>

int part_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    const char *bp = NULL, *bp_end = NULL;
    char label[255] = "";
    tulip_single_note_ctx *begin = NULL, *end = NULL;

    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }

    if (get_cmd_code_from_cmd_tag(buf) != kTlpPart) {
        tlperr_s(error_message, "A part tag was expected.");
        return 0;
    }

    bp = get_next_tlp_technique_block_begin(buf);
    bp_end = get_next_tlp_technique_block_end(buf);

    memset(label, 0, sizeof(label));
    memcpy(label, bp, (bp_end - bp) % sizeof(label));

    bp++;

    while (bp != bp_end) {
        if (!(isascii(*bp) && !is_string_delim(*bp))) {
            tlperr_s(error_message, "The part label has a invalid character : %s", label);
            return 0;
        }
    }

    if (get_tulip_part_ctx(label, get_parts_listing()) != NULL) {
        tlperr_s(error_message, "The part \"%s\" is being redeclared.", label);
    }

    for (end = (*song); end->next != NULL; end = end->next);

    if (end->last != NULL) {
        tlperr_s(error_message, "Insufficient notes to make a part called \"%s\".", label);
        return 0;
    }

    begin = find_oncemore_begin(end);

    set_parts_listing(add_part_to_tulip_part_ctx(get_parts_listing(), label, begin, end));

    (*next) = bp_end + 1;

    return 1;
}
