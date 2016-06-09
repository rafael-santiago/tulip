/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/blockend.h>
#include <dsl/compiler/compiler.h>
#include <dsl/utils.h>
#include <base/ctx.h>

int blockend_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }

    if (get_cmd_code_from_cmd_tag(buf) != kTlpBlockEnd) {
        tlperr_s(error_message, "A block ending token was expected.");
        return 0;
    }

    pop_technique();

    //  INFO(Santiago): D i r t y  t r i c k -> The following code is useless in the song context but handfull for song
    //                                          structures mirroring.
    (*song) = add_note_to_tulip_single_note_ctx((*song), kTlpBlockEnd, NULL);
    (*next) = buf + 1;

    return 1;
}
