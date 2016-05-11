#ifndef TULIP_DSL_UTILS_H
#define TULIP_DSL_UTILS_H 1

#include <base/types.h>

#define is_technique_block_begin(c) ( (c) == '{' )

#define is_technique_block_end(c) ( (c) == '}' )

#define is_technique_tag_announce(c) ( (c) == '.' )

#define is_string_delim(c) ( (c) == '"' )

#define is_string_escaper(c) ( (c) == '\\' )

tulip_command_t get_cmd_code_from_cmd_tag(const char *buf);

int is_single_note(const char *buf);

int is_sustained_technique(const char *buf);

#endif
