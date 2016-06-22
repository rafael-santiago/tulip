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

txttypesetter_sustained_technique_ctx *add_technique_to_txttypesetter_sustained_technique_ctx(txttypesetter_sustained_technique_ctx *techniques);

txttypesetter_sustained_technique_ctx *rm_technique_from_txttypesetter_sustained_technique_ctx(txttypesetter_sustained_technique_ctx *burn, txttypesetter_sustained_technique_ctx *techniques);

void sustain_technique(txttypesetter_sustained_technique_ctx **technique);

void free_txttypesetter_sustained_technique_ctx(txttypesetter_sustained_technique_ctx *techniques);

txttypesetter_tablature_ctx *new_txttypesetter_tablature_ctx(txttypesetter_tablature_ctx **tablature);

void free_txttypesetter_tablature_ctx(txttypesetter_tablature_ctx *tablature);

txttypesetter_active_technique_ctx *push_technique_to_txttypesetter_active_technique_ctx(txttypesetter_active_technique_ctx *active_stack, const tulip_command_t technique, txttypesetter_sustained_technique_ctx **record_list, int *curr_row);

void sustain_active_techniques(txttypesetter_active_technique_ctx *actives, const int dot_nr, const int curr_row);

txttypesetter_active_technique_ctx *pop_technique_from_txttypesetter_active_technique_ctx(txttypesetter_active_technique_ctx *active_stack);

void free_txttypesetter_active_technique_ctx(txttypesetter_active_technique_ctx *active_stack);

#endif
