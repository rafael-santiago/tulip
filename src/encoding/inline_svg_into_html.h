/*
 *                           Copyright (C) 2005-2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_ENCODING_INLINE_SVG_INTO_HTML_H
#define TULIP_ENCODING_INLINE_SVG_INTO_HTML_H 1

#include <stdlib.h>

char *inline_svg_into_html(const char *svg_data, const size_t svg_data_size,
                           size_t *img_tag_size);

#endif
