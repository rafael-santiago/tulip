#ifndef TULIP_DSL_PARSER_VERIFIERS_CHORD_H
#define TULIP_DSL_PARSER_VERIFIERS_CHORD_H 1

#include <base/types.h>

int chord_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, char **next);

#endif
