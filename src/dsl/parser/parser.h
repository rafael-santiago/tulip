#ifndef TULIP_DSL_PARSER_PARSER_H
#define TULIP_DSL_PARSER_PARSER_H 1

char *get_codebuf_from_filepath(const char *filepath);

const char *get_next_tlp_command(const char *codebuf);

const char *get_next_tlp_technique_block(const char *codebuf);

#endif
