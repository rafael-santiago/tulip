/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef PROCESSOR_TYPESETTER_TXT_TXTCTX_H
#define PROCESSOR_TYPESETTER_TXT_TXTCTX_H 1

#include <base/types.h>
#include <processor/typesetters/txt/txttypes.h>

txttypesetter_comment_ctx *add_comment_to_txttypesetter_comment_ctx(txttypesetter_comment_ctx *comments, const char *comment, const size_t fretboard_sz);

void free_txttypesetter_comment_ctx(txttypesetter_comment_ctx *comments);

txttypesetter_sustained_technique_ctx *push_technique_to_txttypesetter_sustained_technique_ctx(txttypesetter_sustained_technique_ctx *techniques, const tulip_command_t technique, const size_t fretboard_sz);

txttypesetter_sustained_technique_ctx *pop_technique_from_txttypesetter_sustained_technique_ctx(txttypesetter_sustained_technique_ctx *techniques);

txttypesetter_sustained_technique_ctx *sustain_techniques(txttypesetter_sustained_technique_ctx *techniques, const size_t fretboard_sz);

void free_txttypesetter_sustained_technique_ctx(txttypesetter_sustained_technique_ctx *techniques);

#endif
