/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/pdf/pdf.h>
#include <processor/typesetters/ps/ps.h>
#include <processor/oututils.h>
#include <stdlib.h>
#include <stdio.h>

int pdf_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    char *tempfile = NULL;
    char cmdline[255];
    int has_error = 0;

    tempfile = tlptemp();

    if (tempfile == NULL) {
        printf("ERROR: unable to generate a temp file.\n");
        has_error = 1;
        goto ___pdf_typesetter_epilogue;
    }

    if ((has_error = ps_typesetter(song, tempfile)) == 0) {
        sprintf(cmdline, "ps2pdf %s %s", tempfile, tabpath);
        has_error = (system(cmdline) != 0);
        if (has_error) {
            printf("ERROR: unable to convert the generated Postscript file to PDF using ps2pdf (exit code=%d).\n", has_error);
        }
        remove(tempfile);
    } else {
        printf("ERROR: during the Postscript processing.\n");
    }

___pdf_typesetter_epilogue:

    return (has_error != 0);
}
