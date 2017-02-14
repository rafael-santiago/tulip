/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/html/html.h>
#include <processor/typesetters/html/htmlinkspill.h>

int html_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    return htmltypesetter_inkspill(tabpath, song);
}
