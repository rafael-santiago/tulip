/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_COMPILER_COMPILER_H
#define TULIP_COMPILER_COMPILER_H 1

#include <base/types.h>

int compile_tulip_codebuf(const char *codebuf, char *message_buf, tulip_single_note_ctx **song);

void tlperr_s(char *buf, const char *error_message, ...);

tulip_command_t get_used_techniques();

void push_technique(const tulip_command_t technique);

void pop_technique();

#endif
