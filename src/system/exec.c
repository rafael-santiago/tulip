/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <system/exec.h>
#include <usrland/cmdlineoptions.h>
#include <dsl/compiler/compiler.h>
#include <base/ctx.h>
#include <system/version.h>
#include <stdio.h>

static int tulip_task_compile(const char *tlp, tulip_single_note_ctx **song);

static int tulip_task_typeset(const char *out, const tulip_single_note_ctx *song);

static int tulip_task_help();

static int tulip_task_version();

static int tulip_task_compile(const char *tlp, tulip_single_note_ctx **song) {
    return 1;
}

static int tulip_task_typeset(const char *out, const tulip_single_note_ctx *song) {
    printf("not yet! comming soon!\n");
    return 1;
}

static int tulip_task_help() {
    printf("use: tulip --tlp=<tlp file path> [--out=<out file path>]\n");
    return 1;
}

static int tulip_task_version() {
    printf("tulip-%s\n", get_tulip_system_version());
    return 0;
}

int tulip_task_exec() {
    const char *tlp = NULL;
    const char *out = NULL;
    tulip_single_note_ctx *song = NULL;
    int exit_code = 1;

    tlp = get_option("tlp", NULL);

    if (tlp != NULL) {
        exit_code = tulip_task_compile(tlp, &song);
        if (exit_code == 0 && (out = get_option("out", NULL)) != NULL) {
            exit_code = tulip_task_typeset(out, song);
            free_tulip_single_note_ctx(song);
        }
    } else if (get_bool_option("version", 0) != 0) {
        exit_code = tulip_task_version();
    } else {
        exit_code = tulip_task_help();
    }

    return exit_code;
}
