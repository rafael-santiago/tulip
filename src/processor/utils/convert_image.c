/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/utils/convert_image.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int convert_image(const char *svg_basename, const char *input_format, const char *output_path) {
    char output_basename[4096], *p;
    char *oext[] = { ".jpeg", ".jpg", ".png" }, *ext = NULL, **oe, **oe_end;
    FILE *convert = NULL;
    int has_error = 1;
    char convcmd[4096], curr_input[4096];
    size_t page_nr = 0;
    int convert_nr = -1;
    struct stat st;

    snprintf(output_basename, sizeof(output_basename) - 1, "%s", output_path);

    oe = &oext[0];
    oe_end = oe + sizeof(oext) / sizeof(oext[0]);

    while (oe != oe_end && ext == NULL) {
        if ((p = strstr(output_basename, *oe)) != NULL) {
            *p = 0;
            ext = *oe;
        }
        oe++;
    }

    if (ext == NULL) {
        fprintf(stderr, "ERROR: Unable to guess up the wanted output image format.\n");
        goto convert_image_epilogue;
    }

    page_nr = 1;

    snprintf(curr_input, sizeof(curr_input) - 1, "%s-%03d.%s", svg_basename, page_nr, input_format);

    while (stat(curr_input, &st) == 0) {
        snprintf(convcmd, sizeof(convcmd) - 1, "convert %s %s-%03d%s", curr_input, output_basename, page_nr, ext);
        has_error = ((convert = popen(convcmd, "r")) == NULL);
        if (has_error) {
            fprintf(stderr, "ERROR: Unable to convert the input. Aborted.\n");
            goto convert_image_epilogue;
        }
        pclose(convert);
        convert = NULL;
        page_nr++;
        snprintf(curr_input, sizeof(curr_input) - 1, "%s-%03d.%s", svg_basename, page_nr, input_format);
    }

convert_image_epilogue:

    return has_error;
}
