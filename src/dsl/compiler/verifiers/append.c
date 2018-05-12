/*
 *                           Copyright (C) 2005-2018 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/parser/parser.h>
#include <dsl/compiler/compiler.h>
#include <dsl/utils.h>
#include <dsl/str/strutils.h>
#include <base/memory.h>
#include <string.h>
#include <stdio.h>

int append_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    tulip_single_note_ctx *sp, *incl_code = NULL;
    const char *bp, *bp_end, *local_next;
    char tlp_path[4096], *exp_tlp_path = NULL;
    FILE *fp = NULL;
    char *tlp_data = NULL;
    size_t tlp_data_size;
    int no_error = 1;
    const char *old_filepath;
    int old_linenumber;

    if (song == NULL || next == NULL || buf == NULL) {
        return 0;
    }

    if (get_cmd_code_from_cmd_tag(buf) != kTlpAppend) {
        tlperr_s(error_message, ".append was expected.");
        no_error = 0;
        goto append_tag_verifier_epilogue;
    }

    bp = get_next_tlp_technique_block_begin(buf);

    if (bp == NULL) {
        tlperr_s(error_message, "Append tag without file path.");
        no_error = 0;
        goto append_tag_verifier_epilogue;
    }

    bp++;

    bp_end = get_next_tlp_technique_block_end(buf);
    (*next) = bp_end + 1;

    if (bp_end == NULL) {
        tlperr_s(error_message, "Unterminated append tag.");
        no_error = 0;
        goto append_tag_verifier_epilogue;
    }

    memset(tlp_path, 0, sizeof(tlp_path));

    while (bp != bp_end && is_blank(*bp)) {
        bp++;
    }

    if (is_blank(*(bp_end - 1))) {
        bp_end -= 1;
        while (bp_end != bp && is_blank(*bp_end)) {
            bp_end--;
        }
        bp_end += 1;
    }

    memcpy(tlp_path, bp, (bp_end - bp) % (sizeof(tlp_path) - 1));

    if (!is_valid_string(tlp_path)) {
        tlperr_s(error_message, "Invalid file path string : %s.", tlp_path);
        no_error = 0;
        goto append_tag_verifier_epilogue;
    }

    exp_tlp_path = expand_string(tlp_path);

    if (exp_tlp_path == NULL) {
        tlperr_s(error_message, "Error while expanding file path string.\n");
        no_error = 0;
        goto append_tag_verifier_epilogue;
    }

    if ((fp = fopen(exp_tlp_path, "r")) == NULL) {
        tlperr_s(error_message, "Unable to open the file \"%s\".", exp_tlp_path);
        no_error = 0;
        goto append_tag_verifier_epilogue;
    }

    fseek(fp, 0L, SEEK_END);
    tlp_data_size = (size_t) ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    tlp_data = (char *) getseg(tlp_data_size + 1);

    memset(tlp_data, 0, tlp_data_size + 1);
    fread(tlp_data, 1, tlp_data_size, fp);
    fclose(fp);
    fp = NULL;

    old_filepath = get_curr_compiled_file();
    old_linenumber = get_curr_code_line_number();

    set_curr_compiled_file(exp_tlp_path);
    set_curr_code_line_number(1);

    no_error = compile_tulip_codebuf(tlp_data, error_message, &incl_code, &local_next);

    set_curr_compiled_file(old_filepath);
    set_curr_code_line_number(old_linenumber);

    if (no_error) {
        for (sp = *song; sp != NULL && sp->next != NULL; sp = sp->next)
            ;

        incl_code->last = sp;

        if (sp == NULL) {
            (*song) = incl_code;
        } else {
            sp->next = incl_code;
        }
    }

append_tag_verifier_epilogue:

    if (fp != NULL) {
        fclose(fp);
    }

    if (exp_tlp_path != NULL) {
        free(exp_tlp_path);
    }

    if (tlp_data != NULL) {
        free(tlp_data);
    }

    return no_error;
}
