/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/md/md.h>
#include <processor/typesetters/md/mdinkspill.h>

int md_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    return mdtypesetter_inkspill(tabpath, song);
}
