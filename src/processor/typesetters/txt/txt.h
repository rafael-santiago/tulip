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

int txt_typesetter(const tulip_single_note_ctx *song, const char *tabpath);

int txttypesetter_get_curr_master_row_value();

int txttypesetter_inc_curr_master_row_value(const int amount);

int txttypesetter_reset_curr_master_row_value();

txttypesetter_tablature_ctx *txttypesetter_get_properly_output_location(txttypesetter_tablature_ctx **tab, const int row_usage);

int txttypesetter_eval_buffer_row_usage(const tulip_command_t techniques, const tulip_single_note_ctx *note, const txttypesetter_tablature_ctx *tab);

#endif
