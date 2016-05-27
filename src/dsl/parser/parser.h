/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_DSL_PARSER_PARSER_H
#define TULIP_DSL_PARSER_PARSER_H 1

#include <stdlib.h>

char *get_codebuf_from_filepath(const char *filepath);

const char *get_next_tlp_command(const char *codebuf);

const char *get_next_tlp_technique_block_begin(const char *codebuf);

const char *get_next_tlp_technique_block_end(const char *codebuf);

const char *skip_string_chunk(const char *codebuf);

size_t get_next_tlp_technique_block_size(const char *codebuf);

void set_curr_code_line_number(const int line_nr);

int get_curr_code_line_number();

#endif
