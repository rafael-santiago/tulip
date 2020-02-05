/*
 *                           Copyright (C) 2005-2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <encoding/inline_font_data.h>
#include <encoding/base64.h>
#include <base/memory.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_CONTENT_TYPE_SIZE 15

static const char *get_mime_type(const char *extension);

char *inline_font_data(const char *filepath, size_t *out_size) {
    char *out = NULL;
    const char *fp, *mtype;
    char *data = NULL, *enc_font = NULL;
    size_t data_size, enc_font_size, mtype_size;
    FILE *font_file = NULL;

    if (out_size != NULL) {
        *out_size = 0;
    }

    if (filepath == NULL) {
        goto inline_font_into_html_epilogue;
    }

    fp = filepath;

    fp = fp + strlen(fp) - 1;

    while (fp != filepath && *fp != '.') {
        fp--;
    }

    if (fp == filepath && *fp != '.') {
        fprintf(stderr, "processor ERROR: Font file name has no extension.\n");
        goto inline_font_into_html_epilogue;
    }

    if ((mtype = get_mime_type(fp)) == NULL) {
        fprintf(stderr, "processor ERROR: Unknown font format : '%s'.\n", fp + 1);
        goto inline_font_into_html_epilogue;
    }

    if ((font_file = fopen(filepath, "rb")) == NULL) {
        fprintf(stderr, "processor ERROR: Unable to open font file '%s'.\n", filepath);
        goto inline_font_into_html_epilogue;
    }

    fseek(font_file, 0L, SEEK_END);
    data_size = ftell(font_file);
    fseek(font_file, 0L, SEEK_SET);

    data = (char *)getseg(data_size +  1);

    fread(data, 1, data_size, font_file);

    enc_font = base64_encode_buffer(data, data_size, &enc_font_size);

    if (enc_font == NULL) {
        fprintf(stderr, "processor ERROR: When encoding font data.\n");
        goto inline_font_into_html_epilogue;
    }

    mtype_size = strlen(mtype);
    data_size = enc_font_size + DEFAULT_CONTENT_TYPE_SIZE + mtype_size;
    out = (char *) getseg(data_size + 1);
    memset(out, 0, data_size + 1);

    if (snprintf(out, data_size, "\"data:%s;base64,", mtype) < 0) {
        fprintf(stderr, "processor ERROR: Unable to embed font data.\n");
        free(out);
        out = NULL;
        goto inline_font_into_html_epilogue;
    }

    memcpy(out + mtype_size + DEFAULT_CONTENT_TYPE_SIZE - 1, enc_font, enc_font_size);
    *(out + mtype_size + DEFAULT_CONTENT_TYPE_SIZE - 1 + enc_font_size) = '"';

inline_font_into_html_epilogue:

    if (font_file != NULL) {
        fclose(font_file);
    }

    if (data != NULL) {
        free(data);
    }

    if (enc_font != NULL) {
        free(enc_font);
    }

    if (out != NULL && out_size != NULL) {
        *out_size = data_size;
    }

    return out;
}

static const char *get_mime_type(const char *extension) {
    const char *type = NULL;
    if (strcmp(extension, ".woff") == 0) {
        type = "application/x-font-woff";
    } else if (strcmp(extension, ".ttf") == 0) {
        type = "application/x-font-ttf";
    } else if (strcmp(extension, ".otf") == 0) {
        type = "application/x-font-opentype";
    } else if (strcmp(extension, ".eot") == 0) {
        type = "application/vnd.ms-fontobject";
    }
    return type;
}
