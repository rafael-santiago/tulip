/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSOR_TYPESETTERS_TXT_TXTTYPES_H
#define TULIP_PROCESSOR_TYPESETTERS_TXT_TXTTYPES_H 1

#include <stdlib.h>

typedef struct _txttypesetter_comment_ctx {
    char *data;
    struct _txttypesetter_comment_ctx *next;
}txttypesetter_comment_ctx;

typedef struct _txttypesetter_sustained_technique_ctx {
    char *data;
    struct _txttypesetter_sustained_technique_ctx *next;
}txttypesetter_sustained_technique_ctx;

typedef struct _txttypesetter_fretboard_ctx {
    txttypesetter_comment_ctx *comments;
    txttypesetter_sustained_technique_ctx *techniques;
    const char *times;
    const char *strings[6];
    const char tunning[6][3];
    struct _txttypesetter_fretboard_ctx *next;
}txttypesetter_fretboard_ctx;

#endif
