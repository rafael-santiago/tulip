/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSOR_TYPESETTERS_SVG_SVGINKSPILL_H
#define TULIP_PROCESSOR_TYPESETTERS_SVG_SVGINKSPILL_H 1

#include <processor/typesetters/txt/txttypes.h>

int svgtypesetter_inkspill(const char *filepath, txttypesetter_tablature_ctx *tab,
                           const tulip_single_note_ctx *song);

#endif

