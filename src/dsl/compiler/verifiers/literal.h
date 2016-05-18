#ifndef DSL_COMPILER_VERIFIERS_LITERAL_H
#define DSL_COMPILER_VERIFIERS_LITERAL_H 1

#include <base/types.h>

int literal_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

#endif
