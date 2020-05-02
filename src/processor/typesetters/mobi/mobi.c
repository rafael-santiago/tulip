/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/mobi/mobi.h>
#include <processor/typesetters/mobi/mobiinkspill.h>

int mobi_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    return mobitypesetter_inkspill(tabpath, song);
}
