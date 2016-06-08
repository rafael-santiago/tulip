/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/transcriber.h>
#include <dsl/compiler/verifiers/datatag.h>
#include <stdlib.h>

int transcriber_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {
    tulip_single_note_ctx *sp = NULL;
    if (song == NULL) {
        return 0;
    }
    for (sp = (*song); sp != NULL; sp = sp->next) {
        if (sp->techniques == kTlpTranscriber) {
            tlperr_s(error_message, "The transcriber tag is being used twice.");
            return 0;
        }
    }
    return datatag_verifier(kTlpTranscriber, buf, error_message, song, next);
}
