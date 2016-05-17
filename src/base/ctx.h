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

#endif
