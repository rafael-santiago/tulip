#ifndef TULIP_DSL_UTILS_H
#define TULIP_DSL_UTILS_H 1

#include <base/types.h>
#include <ctype.h>

#define is_technique_block_begin(c) ( (c) == '{' )

#define is_technique_block_end(c) ( (c) == '}' )

#define is_technique_tag_announce(c) ( (c) == '.' )

#define is_sep(c) ( (c) == '-' )

#define is_hammeron(c) ( (c) == 'h' )

#define is_pulloff(c) ( (c) == 'p' )

#define is_vibrato(c) ( (c) == '~' )

#define is_slidedown(c) ( (c) == '/' )

#define is_slideup(c) ( (c) == '\\' )

#define is_note(c) ( ( (c) >= '1' && (c) <= '6' ) )

#define is_string_delim(c) ( (c) == '"' )

#define is_string_escaper(c) ( (c) == '\\' )

#define is_blank(c) ( (c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n' )

#define is_save_point(c) ( (c) == ';' )

#define is_sep_bar(c) ( (c) == '|' )

#define is_bend(c) ( (c) == 'b' )

#define is_release_bend(c) ( (c) == 'r' )

tulip_command_t get_cmd_code_from_cmd_tag(const char *buf);

int is_single_note(const char *buf);

int is_sustained_technique(const char *buf);

#endif
