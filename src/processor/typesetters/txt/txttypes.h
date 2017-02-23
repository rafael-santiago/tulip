/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSOR_TYPESETTERS_TXT_TXTTYPES_H
#define TULIP_PROCESSOR_TYPESETTERS_TXT_TXTTYPES_H 1

#include <base/types.h>
#include <stdlib.h>

typedef struct _txttypesetter_comment_ctx {
    char *data;
    struct _txttypesetter_comment_ctx *next;
}txttypesetter_comment_ctx;

typedef struct _txttypesetter_sustained_technique_ctx {
    char *data;
    struct _txttypesetter_sustained_technique_ctx *next, *last;
}txttypesetter_sustained_technique_ctx;

typedef struct _txttypesetter_active_technique_ctx {
    tulip_command_t technique;
    char *dp;
    struct _txttypesetter_active_technique_ctx *next;
}txttypesetter_active_technique_ctx;

typedef struct _txttypesetter_tablature_ctx {
    txttypesetter_comment_ctx *comments;
    txttypesetter_sustained_technique_ctx *techniques;
    txttypesetter_active_technique_ctx *active_techniques;
    char *times;
    char *strings[6];
    char tuning[6][4];
    char *song;
    char *transcriber;
    size_t curr_row;
    size_t curr_str;
    size_t fretboard_sz;
    size_t string_nr;
    struct _txttypesetter_tablature_ctx *next, *last;
}txttypesetter_tablature_ctx;

#endif
