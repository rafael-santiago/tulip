/*
 *                           Copyright (C) 2005-2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <encoding/inline_svg_into_html.h>
#include <encoding/base64.h>
#include <base/memory.h>
#include <stdio.h>
#include <string.h>

#define SVG_DATA_CONTENT_TYPE "data:image/svg+xml;base64,"
#define SVG_DATA_CONTENT_TYPE_LEN 26

char *inline_svg_into_html(const char *svg_data, const size_t svg_data_size,
                           size_t *img_tag_size) {
    unsigned char *enc_image;
    size_t enc_image_size;
    size_t img_size;
    char *img;

    if (svg_data == NULL || svg_data_size == 0) {
        return NULL;
    }

    if ((enc_image = base64_encode_buffer(svg_data, svg_data_size, &enc_image_size)) == NULL) {
        return NULL;
    }

    img_size = enc_image_size + SVG_DATA_CONTENT_TYPE_LEN + 14;
    img = (char *) getseg(img_size + 1);
    memset(img, 0, img_size + 1);

    snprintf(img, img_size, "<img src = \"%s", SVG_DATA_CONTENT_TYPE);
    memcpy(img + SVG_DATA_CONTENT_TYPE_LEN + 12, enc_image, enc_image_size);
    memcpy(img + SVG_DATA_CONTENT_TYPE_LEN + 12 + enc_image_size, "\">", 2);

    if (img_tag_size != NULL) {
        *img_tag_size = img_size;
    }

    free(enc_image);

    return img;
}

#undef SVG_DATA_CONTENT_TYPE
#undef SVG_DATA_CONTENT_TYPE_LEN

