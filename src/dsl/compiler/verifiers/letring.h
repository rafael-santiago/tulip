#ifndef TULIP_DSL_COMPILER_VERIFIERS_LETRING_H
#define TULIP_DSL_COMPILER_VERIFIERS_LETRING_H 1

#include <base/types.h>

int letring_tag_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

#endif