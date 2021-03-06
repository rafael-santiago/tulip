/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSOR_TYPESETTERS_TXT_TXT_H
#define TULIP_PROCESSOR_TYPESETTERS_TXT_TXT_H 1

#include <base/types.h>
#include <processor/typesetters/txt/txttypes.h>

txttypesetter_tablature_ctx *txt_typesetter_gettab(const tulip_single_note_ctx *song);

void apply_final_output_brush_up(txttypesetter_tablature_ctx *tab);

int txt_typesetter(const tulip_single_note_ctx *song, const char *tabpath);

txttypesetter_tablature_ctx *txttypesetter_get_properly_output_location(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note, const int row_usage);

int txttypesetter_eval_buffer_row_usage(const tulip_command_t techniques, const tulip_single_note_ctx *note, const txttypesetter_tablature_ctx *tab);

void txttypesetter_print_sustained_technique_mark(const tulip_command_t command, txttypesetter_tablature_ctx **tab, const int row_usage);

#endif
