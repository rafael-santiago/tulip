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
#include <processor/processor.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int tulip_task_compile(const char *tlp, tulip_single_note_ctx **song);

static int tulip_task_typeset(const char *out, const tulip_single_note_ctx *song);

static int tulip_task_help();

static int tulip_task_version();

static int tulip_task_notask();

static int tulip_task_compile(const char *tlp, tulip_single_note_ctx **song) {
    char errmsg[255];
    char *tlpcode = NULL;
    long tlpcode_sz = 0;
    FILE *fp = NULL;
    int is_tlpcode_ok = 0;

    if (tlp == NULL || song == NULL) {
        printf("tulip system PANIC: Hey! Butt-Head... He! He! We have got a NULL @song here!!! "
               "Hu-Huh uhhhhhhh it is bad... Yeah!! HeheHuhehUhhuhehehe... look! look!! or maybe a NULL @tlp too "
               "Oh! NULL!! Huh-huh... Yeah, yeah Huhuhehuehuehueheuheuheuheuuheuheuheuheuhehuehueheuh...\n");
        return 1;
    }

    if ((fp = fopen(tlp, "rb")) == NULL) {
        printf("tulip I/O ERROR: unable to read from file \"%s\".\n", tlp);
        return 1;
    }

    fseek(fp, 0L, SEEK_END);
    tlpcode_sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    tlpcode = (char *) getseg(tlpcode_sz + 1);
    memset(tlpcode, 0, tlpcode_sz + 1);
    if (fread(tlpcode, 1, tlpcode_sz, fp) == -1) {
        printf("tulip I/O ERROR: during the file reading process.\n");
        fclose(fp);
        free(tlpcode);
        return 1;
    }

    fclose(fp);

    is_tlpcode_ok = compile_tulip_codebuf(tlpcode, errmsg, song);

    free(tlpcode);

    if (!is_tlpcode_ok) {
        printf("%s", errmsg);
        return 1;
    }

    return 0;
}

static int tulip_task_typeset(const char *out, const tulip_single_note_ctx *song) {
    return mktab(song, out);
}

static int tulip_task_help() {
    printf("usage: tulip --tlp=<tlp file path> [--out=<out file path>]\n\n");
    printf("* If you do not know Tulip's Language try to take a look at MANUAL.md.\n"
           "  This is located under the sub-path doc/MANUAL.md in the repo's tree.\n\n"
           "Tulip is licensed under GPLv2. This is a free software.\n"
           "You can redistribute it and/or modify under the terms of the GNU General Public License version 2.\n\n"
           "Tulip language is Copyright (C) 2005-2016 by Rafael Santiago.\n"
           "Tulip compiler is Copyright (C) 2005-2016 by Rafael Santiago.\n"
           "Tulip txt typesetter is Copyright (C) 2005-2016 by Rafael Santiago.\n\n"
           "Bug reports, feedback, etc: <voidbrainvoid@gmail.com> or <https://github.com/rafael-santiago/tulip/issues>\n\n");
    return 0;
}

static int tulip_task_notask() {
    printf("tulip: What do you mean? Try to call me again using --help option. Maybe we will can understand each other in a better way.\n");
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
    } else if (get_bool_option("help", 0) != 0) {
        exit_code = tulip_task_help();
    } else {
        exit_code = tulip_task_notask();
    }

    return exit_code;
}
