/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/jpeg/jpeg.h>
#include <processor/typesetters/svg/svg.h>
#include <processor/utils/has_convert.h>
#include <processor/utils/get_temp_filename.h>
#include <processor/utils/convert_image.h>
#include <processor/utils/remove_svg_pages.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int jpeg_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    int has_error = 1;
    char tempsvg[70];
    char temppath[70];

    memset(tempsvg, 0, sizeof(tempsvg));

    if (song == NULL || tabpath == NULL) {
        goto jpeg_typesetter_epilogue;
    }

    if (!has_convert()) {
        fprintf(stderr, "ERROR: The system seems not to has convert from ImageMagick suite installed.\n"
                        "       Install it by exporting its path properly and try again.\n");
        goto jpeg_typesetter_epilogue;
    }

    if (get_temp_filename(tempsvg, sizeof(tempsvg) - 1, "tmpSvg", 6) == NULL) {
        fprintf(stderr, "ERROR: Unable to get a valid temporary file name.\n");
        has_error = errno;
        goto jpeg_typesetter_epilogue;
    }

    snprintf(temppath, sizeof(temppath) - 1, "%s.svg", tempsvg);

    if ((has_error = svg_typesetter(song, temppath)) != 0) {
        goto jpeg_typesetter_epilogue;
    }

    has_error = convert_image(tempsvg, "svg", tabpath);

jpeg_typesetter_epilogue:

    if (has_error != 0 && remove_svg_pages(tempsvg) == 0) {
        fprintf(stderr, "WARN: Unable to remove temporary SVG data generated.\n"
                        "      Do it yourself. The used base name was '%s'.\n", tempsvg);
    }

    return has_error;
}

int jpg_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    return jpeg_typesetter(song, tabpath);
}
