/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/repeat.h>
#include <dsl/compiler/verifiers/oncemore.h>
#include <dsl/compiler/compiler.h>
#include <dsl/parser/parser.h>
#include <dsl/utils.h>
#include <base/ctx.h>
#include <string.h>

typedef int (*verifier_t)(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

typedef enum {
    kTlCheckIni,
    kTlCheckNumber,
    kTlCheckDot,
    kTlCheckHalf,
    kTlCheckFinis,
    kTlcheckMachineStatesNr
}tlcheck_machine_state_t;

typedef int (*tlcheck_states)(const char **lp, const char *lp_end, tlcheck_machine_state_t *);

static int tlcheck_ms_ini(const char **lp, const char *lp_end, tlcheck_machine_state_t *curr_state);

static int tlcheck_ms_number(const char **lp, const char *lp_end, tlcheck_machine_state_t *curr_state);

static int tlcheck_ms_dot(const char **lp, const char *lp_end, tlcheck_machine_state_t *curr_state);

static int tlcheck_ms_half(const char **lp, const char *lp_end, tlcheck_machine_state_t *curr_state);

static int tlcheck_ms_finis(const char **lp, const char *lp_end, tlcheck_machine_state_t *curr_state);

static int no_repeat_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

static int no_code_listing_repeat_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song,
                                           const char **next);

static int unterminated_repeat_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song,
                                            const char **next);

static int v6_repeat_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

static int v7_repeat_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

static verifier_t get_suitable_repeat_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song,
                                                   const char **next);

static void get_repeat_data(char *data, const size_t data_size, const char *buf);

static int check_transposition_level(const char *level, const size_t level_size);

int repeat_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    return get_suitable_repeat_tag_verifier(buf, error_message, song, next)(buf, error_message, song, next);
}

static verifier_t get_suitable_repeat_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song,
                                                   const char **next) {
    const char *bp_end;

    if (buf == NULL || song == NULL || next == NULL || get_cmd_code_from_cmd_tag(buf) != kTlpRepeat) {
        return no_repeat_tag_verifier;
    }

    if (get_next_tlp_technique_block_begin(buf) == NULL) {
        return no_code_listing_repeat_verifier;
    }

    if ((bp_end = get_next_tlp_technique_block_end(buf)) == NULL) {
        return unterminated_repeat_tag_verifier;
    }

    bp_end++;
    while (is_blank(*bp_end)) {
        bp_end++;
    }

    if (is_technique_block_begin(*bp_end)) {
        // INFO(Rafael): It has used repeat tag in the way introduced from v7 ('.repeat{part-label}{<transposition level>}').
        return v7_repeat_tag_verifier;
    }

    return v6_repeat_tag_verifier; // INFO(Rafael): It seems a code using v6's syntax or older.
}

static int no_repeat_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    tlperr_s(error_message, "A repeat tag was expected.");
    return 0;
}

static int unterminated_repeat_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song,
                                            const char **next) {
    tlperr_s(error_message, "Unterminated tag repeat.");
    return 0;
}

static int no_code_listing_repeat_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song,
                                               const char **next) {
    tlperr_s(error_message, "A tag repeat without code listing.");
    return 0;
}

static void get_repeat_data(char *data, const size_t data_size, const char *buf) {
    const char *bp = NULL, *bp_end = NULL;

    bp = get_next_tlp_technique_block_begin(buf);
    bp_end = get_next_tlp_technique_block_end(buf);

    memset(data, 0, data_size);
    bp++;
    memcpy(data, bp, (bp_end - bp) % data_size);
}

static int v6_repeat_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    char label[255] = "";
    tulip_part_ctx *part = NULL;

    get_repeat_data(label, sizeof(label) - 1, buf);

    if ((part = get_tulip_part_ctx(label, get_parts_listing())) == NULL) {
        tlperr_s(error_message, "An undefined part \"%s\" was mentioned.", label);
        return 0;
    }

    tulip_single_note_ctx_cpy(song, part->begin, part->end);
    (*next) = get_next_tlp_technique_block_end(buf) + 1;

    return 1;
}

static int v7_repeat_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    const char *bp = NULL, *bp_end = NULL;
    char label[255] = "", tlevel[255] = "";
    tulip_part_ctx *part = NULL;

    get_repeat_data(label, sizeof(label) - 1, buf);

    bp = get_next_tlp_technique_block_end(buf) + 1;
    bp = get_next_tlp_technique_block_begin(bp);

    get_repeat_data(tlevel, sizeof(tlevel) - 1, bp);

    if (!check_transposition_level(tlevel, strlen(tlevel))) {
        tlperr_s(error_message, "The given transposition level is invalid : \"%s\".\n", tlevel);
        return 0;
    }

    if ((part = get_tulip_part_ctx(label, get_parts_listing())) == NULL) {
        tlperr_s(error_message, "An undefined part \"%s\" was mentioned.", label);
        return 0;
    }

    // TODO(Rafael): Transposition stuff.

    tulip_single_note_ctx_cpy(song, part->begin, part->end);
    (*next) = get_next_tlp_technique_block_end(bp) + 1;

    return 1;
}

static int tlcheck_ms_ini(const char **lp, const char *lp_end, tlcheck_machine_state_t *curr_state) {
    int out = 0;

    if (*(*lp) == '+' || *(*lp) == '-') {
        (*lp) += 1;
        *curr_state = kTlCheckNumber;
        out = 1;
    }

    return out;
}

static int tlcheck_ms_number(const char **lp, const char *lp_end, tlcheck_machine_state_t *curr_state) {
    int out = 0;

    if ((*lp) < lp_end && isdigit(*(*lp))) {
        (*lp)++;
        if ((*lp) == lp_end) {
            out = 1;
            *curr_state = kTlCheckFinis;
        } else if (*(*lp) == '.') {
            out = 1;
            *curr_state = kTlCheckDot;
        }
    }

    return out;
}

static int tlcheck_ms_dot(const char **lp, const char *lp_end, tlcheck_machine_state_t *curr_state) {
    int out = 0;

    if (*(*lp) == '.') {
        (*lp) += 1;
        *curr_state = kTlCheckHalf;
        out = 1;
    }

    return out;
}

static int tlcheck_ms_half(const char **lp, const char *lp_end, tlcheck_machine_state_t *curr_state) {
    int out = 0;

    if (*(*lp) == '5') {
        (*lp) += 1;
        *curr_state = kTlCheckFinis;
        out = 1;
    }

    return out;
}

static int tlcheck_ms_finis(const char **lp, const char *lp_end, tlcheck_machine_state_t *curr_state) {
    int out = 0;

    if ((*lp) == lp_end) {
        out = 1;
    }

    return out;
}

static int check_transposition_level(const char *level, const size_t level_size) {
    const char *lp, *lp_end;
    tlcheck_states tlcheck_machine[] = {
        // INFO(Rafael): This following tiny Turing machine accepts: "+" | "-" <number> (<number>* | ".5") $
        //
        //               ...and, yes, I could had checked it with just a single traversing loop, but...
        //
        //               MauhAuhau
        //               Muahauhauhauahuah
        //               Muahuhauahuahhauhauhauahuahauhauahuahauhauahuahauahuahauahuahauahau!
        //
        //               I love Turing machines, and Alan Turing rocks!
        tlcheck_ms_ini,
        tlcheck_ms_number,
        tlcheck_ms_dot,
        tlcheck_ms_half,
        tlcheck_ms_finis
    };
    tlcheck_machine_state_t curr_state;
    int m_out;

    if (level == NULL || level_size == 0) {
        return 0;
    }

    lp = level;
    lp_end = lp + level_size;

    curr_state = kTlCheckIni;

    do {
        m_out = tlcheck_machine[curr_state](&lp, lp_end, &curr_state);
    } while (curr_state != kTlCheckFinis && m_out != 0);

    return m_out;
}
