#ifndef TULIP_DSL_COMPILER_VERIFIERS_PART_H
#define TULIP_DSL_COMPILER_VERIFIERS_PART_H 1

#include <base/types.h>

int part_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

#endif
