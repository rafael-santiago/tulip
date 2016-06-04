/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/undo_recursion.h>
#include <dsl/compiler/compiler.h>
#include <dsl/utils.h>
#include <dsl/parser/parser.h>
#include <base/ctx.h>

int undo_recursion(const tulip_command_t command, const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    const char *bp_end = NULL;
    const char *bp = NULL;

    bp = get_next_tlp_technique_block_begin(buf);
    bp_end = get_next_tlp_technique_block_end(buf);

    push_technique(command);

    (*song) = add_note_to_tulip_single_note_ctx((*song), get_used_techniques() | command, NULL);

    return compile_tulip_codebuf(bp, error_message, song, next);
}
