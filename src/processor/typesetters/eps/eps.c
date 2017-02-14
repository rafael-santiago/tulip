/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/eps/eps.h>
#include <processor/typesetters/eps/epsinkspill.h>

int eps_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    return epstypesetter_inkspill(tabpath, song);
}
