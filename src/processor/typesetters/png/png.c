/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/png/png.h>
#include <processor/typesetters/imagick.h>

int png_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    return imagick_typesetter(song, tabpath);
}
