/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <usrland/cmdlineoptions.h>
#include <base/libc/string.h>

static int g_argc = 0;

static char **g_argv = NULL;

static const char *get_option_value(const char *option, int boolean_option, const char *default_value);

void set_option_argc_argv(const int argc, char **argv) {
    g_argc = argc;
    g_argv = argv;
}

static const char *get_option_value(const char *option, int boolean_option, const char *default_value) {
    char temp[255] = "";
    int a = 0;
    if (option == NULL || g_argv == NULL) {
        return default_value;
    }
    strncpy(temp, "--", sizeof(temp) - 1);
    strncat(temp, option, sizeof(temp) - 1);
    if (!boolean_option) {
        strncat(temp, "=", sizeof(temp) - 1);
    }
    for (a = 0; a < g_argc; a++) {
        if (boolean_option) {
            if (strcmp(g_argv[a], temp) == 0) {
                return "1";
            }
        } else if (strstr(g_argv[a], temp) == g_argv[a]) {
            return g_argv[a] + strnlen(temp, sizeof(temp) - 1);
        }
    }
    return default_value;
}


const char *get_option(const char *option, const char *default_value) {
    return get_option_value(option, 0, default_value);
}

int get_bool_option(const char *option, const int default_value) {
    return get_option_value(option, 1, NULL) == NULL ? default_value : 1;
}
