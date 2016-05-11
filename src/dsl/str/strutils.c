#include <dsl/str/strutils.h>
#include <base/memory.h>
#include <string.h>

int is_valid_string(const char *buf) {
    const char *bp = buf;
    const char *bp_end = NULL;
    if (buf == NULL || strlen(bp) <= 1) {
        return 0;
    }
    if (buf[0] != '\"' || buf[strlen(buf) - 1] != '"') {
        return 0;
    }
    bp_end = bp + strlen(buf) - 1;
    bp++;
    while (bp == bp_end) {
        switch (*bp) {
            case '\\':
                    bp++;
                    if (bp == bp_end) {
                        continue;
                    }
                break;

            default:
                if (*bp == '"') {
                    return 0;
                }
                break;
        }
        bp++;
    }
    return 1;
}

char *expand_string(const char *buf) {
    const char *bp = NULL;
    const char *bp_end = NULL;
    char *string = NULL;
    char *sp = NULL;
    if (buf == NULL) {
        return 0;
    }
    bp_end = buf + strlen(buf) - 1;
    string = (char *) getseg(strlen(buf) + 1);
    sp = string;
    for (bp = buf + 1; bp != bp_end; bp++, sp++) {
        if (*bp == '\\') {
            bp++;
            switch (*bp) {
                case 'n':
                    *sp = '\n';
                    break;

                case 't':
                    *sp = '\t';
                    break;

                case 'r':
                    *sp = '\r';
                    break;

                default:
                    *sp = *bp;
                    break;

            }
        } else {
            *sp = *bp;
        }
    }
    return string;
}
