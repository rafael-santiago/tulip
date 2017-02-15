/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/meta.h>
#include <processor/typesetters/txt/txtctx.h>
#include <processor/typesetters/txt/txt.h>

int metatypesetter_inkspill(const char *tabpath, const tulip_single_note_ctx *song,
                            const char *linebreaking,
                            song_title_spill_func metatypesetter_spill_song_title,
                            transcribers_name_spill_func metatypesetter_spill_transcribers_name,
                            tab_notation_spill_func metatypesetter_spill_tab_notation,
                            tunning_spill_func metatypesetter_spill_tunning,
                            song_spill_func metatypesetter_spill_song) {
    int has_error = 0;
    FILE *fp = fopen(tabpath, "w");
    txttypesetter_tablature_ctx *txttab = NULL;

    if (fp == NULL) {
        has_error = 1;
        goto ___metatypesetter_inkspill_epilogue;
    }

    if ((txttab = txt_typesetter_gettab(song)) == NULL) {
        has_error = 1;
        goto ___metatypesetter_inkspill_epilogue;
    }

    if (metatypesetter_spill_song_title != NULL) {
        metatypesetter_spill_song_title(fp, txttab->song);
    }

    if (metatypesetter_spill_transcribers_name != NULL) {
        metatypesetter_spill_transcribers_name(fp, txttab->transcriber);
    }

    if (linebreaking != NULL && txttab->song != NULL || txttab->transcriber != NULL) {
        fprintf(fp, "%s", linebreaking);
    }

    free_txttypesetter_tablature_ctx(txttab);

    if (metatypesetter_spill_tab_notation != NULL) {
        metatypesetter_spill_tab_notation(fp, song);
    }

    if (metatypesetter_spill_tunning != NULL) {
        metatypesetter_spill_tunning(fp);
    }

    if (metatypesetter_spill_song != NULL) {
        metatypesetter_spill_song(fp, song);
    }

___metatypesetter_inkspill_epilogue:

    if (fp != NULL) {
        fclose(fp);
    }

    return has_error;
}
