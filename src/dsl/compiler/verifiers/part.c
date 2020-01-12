/*
 *                           Copyright (C) 2005-2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/part.h>
#include <dsl/compiler/verifiers/oncemore.h>
#include <dsl/compiler/compiler.h>
#include <dsl/utils.h>
#include <dsl/parser/parser.h>
#include <base/ctx.h>
#include <base/memory.h>
#include <ctype.h>
#include <string.h>

typedef int (*verifier_t)(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

static verifier_t get_suitable_tag_verifier(const char *buf, tulip_single_note_ctx **song, const char **next);

static int v6_part_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

static int v7_part_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

static int no_part_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

static int no_code_listing_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song,
                                        const char **next);

static int unterminated_part_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song,
                                          const char **next);

int part_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    return get_suitable_tag_verifier(buf, song, next)(buf, error_message, song, next);
}

static verifier_t get_suitable_tag_verifier(const char *buf, tulip_single_note_ctx **song, const char **next) {
    const char *bp_end;

    if (buf == NULL || song == NULL || next == NULL || get_cmd_code_from_cmd_tag(buf) != kTlpPart) {
        return no_part_tag_verifier;
    }

    if (get_next_tlp_technique_block_begin(buf) == NULL) {
        return no_code_listing_tag_verifier;
    }

    if ((bp_end = get_next_tlp_technique_block_end(buf)) == NULL) {
        return unterminated_part_tag_verifier;
    }

    bp_end++;
    while (is_blank(*bp_end)) {
        bp_end++;
    }

    if (is_technique_block_begin(*bp_end)) {
        // INFO(Rafael): It has used part tag in the way introduced from v7 ('.part{part-label}{tlp-code}').
        return v7_part_tag_verifier;
    }

    return v6_part_tag_verifier; // INFO(Rafael): It seems a code using v6's syntax or older.
}

static int no_part_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    tlperr_s(error_message, "A part tag was expected.");
    return 0;
}

static int no_code_listing_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song,
                                        const char **next) {
    tlperr_s(error_message, "A tag part without code listing.");
    return 0;
}

static int unterminated_part_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song,
                                          const char **next) {
    tlperr_s(error_message, "Unterminated part tag.");
    return 0;
}

static int get_part_label(char *label, const size_t label_size, const char *buf, char *error_message) {
    const char *bp = NULL, *bp_end = NULL;

    bp = get_next_tlp_technique_block_begin(buf);
    bp_end = get_next_tlp_technique_block_end(buf);

    memset(label, 0, label_size);
    bp++;
    memcpy(label, bp, (bp_end - bp) % label_size);

    while (bp != bp_end) {
        if (!(isascii(*bp) && !is_string_delim(*bp) && !is_technique_block_begin(*bp) && !is_technique_block_end(*bp))) {
            tlperr_s(error_message, "The part label has a invalid character : %s", label);
            return 0;
        }
        bp++;
    }

    if (get_tulip_part_ctx(label, get_parts_listing()) != NULL) {
        tlperr_s(error_message, "The part \"%s\" is begin redeclared.", label);
        return 0;
    }

    if (get_tulip_part_ctx(label, get_parts_listing()) != NULL) {
        tlperr_s(error_message, "The part \"%s\" is being redeclared.", label);
        return 0;
    }

    return 1;
}

static int v7_part_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    const char *bp, *bp_end, *local_next;
    char label[255] = "", *tlpdata = NULL;
    tulip_single_note_ctx *begin = NULL, *end = NULL;
    size_t tlpdata_size;
    int no_error;

    if (get_part_label(label, sizeof(label), buf, error_message) == 0) {
        return 0;
    }

    bp = get_next_tlp_technique_block_end(buf);
    bp = get_next_tlp_technique_block_begin(bp);
    if (bp == NULL) {
        tlperr_s(error_message, "Unable to get code chunk from part tag.");
        return 0;
    }

    bp_end = get_next_tlp_technique_block_end(bp);
    if (bp_end == NULL) {
        tlperr_s(error_message, "Part tag with unterminated code chunk.");
        return 0;
    }

    tlpdata_size = bp_end - bp;
    tlpdata = (char *) getseg(tlpdata_size + 1);
    memset(tlpdata, 0, tlpdata_size + 1);
    memcpy(tlpdata, bp, tlpdata_size);

    if ((*song) != NULL) {
        for (begin = (*song); begin->next != NULL; begin = begin->next)
            ;
    }

    no_error = compile_tulip_codebuf(tlpdata, error_message, song, &local_next);

    if (no_error) {
        begin = (begin != NULL) ? begin->next : (*song);

        for (end = begin; end->next != NULL; end = end->next)
            ;

        set_parts_listing(add_part_to_tulip_part_ctx(get_parts_listing(), label, begin, end));

        (*next) = bp_end + 1;
    }

    free(tlpdata);

    return no_error;
}

static int v6_part_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    char label[255] = "";
    tulip_single_note_ctx *begin = NULL, *end = NULL;

    if (get_part_label(label, sizeof(label), buf, error_message) == 0) {
        return 0;
    }

    if ((*song) == NULL) {
        tlperr_s(error_message, "There is nothing to be marked as a part here.");
        return 0;
    }

    for (end = (*song); end->next != NULL; end = end->next)
        ;

    if (end->last == NULL) {
        tlperr_s(error_message, "Insufficient notes to make a part called \"%s\".", label);
        return 0;
    }

    begin = find_oncemore_begin(end);

    set_parts_listing(add_part_to_tulip_part_ctx(get_parts_listing(), label, begin, end));

    (*next) = get_next_tlp_technique_block_end(buf) + 1;

    return 1;
}
