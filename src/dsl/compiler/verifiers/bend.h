#ifndef DSL_COMPILER_VERIFIERS_BEND_H
#define DSL_COMPILER_VERIFIERS_BEND_H 1

#include <base/types.h>

int bend_sep_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next);

#endif
