#ifndef TULIP_DSL_COMPILER_VERIFIERS_SAVEPOINT_H
#define TULIP_DSL_COMPILER_VERIFIERS_SAVEPOINT_H 1

#include <base/types.h>

int savepoint_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

#endif
