#ifndef TULIP_COMPILER_COMPILER_H
#define TULIP_COMPILER_COMPILER_H 1

#include <base/types.h>

tulip_single_note_ctx *compile_tulip_codebuf(const char *codebuf, char *message_buf);

void tlperr_s(char *buf, const char *error_message, ...);

tulip_command_t get_used_techniques();

void push_technique(const tulip_command_t technique);

void pop_technique();

#endif
