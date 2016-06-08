/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_BASE_CTX_H
#define TULIP_BASE_CTX_H 1

#include <base/types.h>

tulip_technique_stack_ctx *push_technique_to_technique_stack_ctx(tulip_technique_stack_ctx *stack, tulip_command_t technique);

tulip_technique_stack_ctx *pop_technique_from_technique_stack_ctx(tulip_technique_stack_ctx *stack);

int is_empty_technique_stack_ctx(const tulip_technique_stack_ctx *stack);

tulip_command_t top_of_technique_stack_ctx(const tulip_technique_stack_ctx *stack);

void free_technique_stack_ctx(tulip_technique_stack_ctx *stack);

tulip_single_note_ctx *add_note_to_tulip_single_note_ctx(tulip_single_note_ctx *song, tulip_command_t techniques, const char *buf);

void free_tulip_single_note_ctx(tulip_single_note_ctx *song);

tulip_part_ctx *get_tulip_part_ctx(const char *label, tulip_part_ctx *parts);

tulip_part_ctx *add_part_to_tulip_part_ctx(tulip_part_ctx *parts, const char *label, const tulip_single_note_ctx *begin, const tulip_single_note_ctx *end);

void free_tulip_part_ctx(tulip_part_ctx *parts);

#endif
