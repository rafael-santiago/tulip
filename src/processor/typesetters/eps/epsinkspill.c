/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/eps/epsinkspill.h>
#include <processor/typesetters/ps/psinkspill.h>
#include <processor/typesetters/ps/ps.h>
#include <processor/oututils.h>
#include <system/version.h>
#include <base/memory.h>
#include <stdio.h>
#include <string.h>

#define EPSTYPESETTER_BOTTOM_SPACER 100

static void epstypesetter_mkheader(FILE *fp, const int carriage_y);

static char *epstypesetter_getpsdata(const char *tempfile, size_t *dsize);

static void epstypesetter_mktrailer(FILE *fp);

static int epstypesetter_get_page_nr(const char *psdata);

static int epstypesetter_eval_bounding_box_lly(const int carriage_y);

static void epstypesetter_mkheader(FILE *fp, const int carriage_y) {
    fprintf(fp, "%%!PS-Adobe-2.0 EPSF-2.0\n"
                "%%%%BoundingBox: 40 %d 550 734\n"
//                "%%%%HiResBoundingBox: 40.000000 30.000000 560.500000 733.500000\n"
                "%%Generated with tulip-%s\n"
                "%%%%EndComments\n"
                "%%%%BeginProlog\n"
                "save\n"
                "countdictstack\n"
                "mark\n"
                "newpath\n"
                "/showpage {} def\n"
                "/setpagedevice {pop} def\n"
                "%%%%EndProlog\n", epstypesetter_eval_bounding_box_lly(carriage_y), get_tulip_system_version());
}

static int epstypesetter_eval_bounding_box_lly(const int carriage_y) {
    return (carriage_y - EPSTYPESETTER_BOTTOM_SPACER);
}

static char *epstypesetter_getpsdata(const char *tempfile, size_t *dsize) {
    FILE *fp = NULL;
    char *data = NULL;
    long fsize = 0;

    if ((fp = fopen(tempfile, "r")) != NULL) {
        fseek(fp, 0L, SEEK_END);
        fsize = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        data = getseg(fsize + 1);
        memset(data, 0, fsize + 1);
        fread(data, 1, fsize, fp);
        fclose(fp);
        if (dsize != NULL) {
            *dsize = fsize;
        }
    }

    return data;
}

static void epstypesetter_mktrailer(FILE *fp) {
    fprintf(fp, "%%%%Trailer\n"
                "cleartomark\n"
                "countdictstack\n"
                "exch sub { end } repeat\n"
                "restore\n"
                "%%%%EOF\n");
}

static int epstypesetter_get_page_nr(const char *psdata) {
    int nr = 0;
    const char *p = NULL, *p_end = NULL;

    if (psdata == NULL) {
        return 0;
    }

    p = psdata;
    p_end = p + strlen(p);

    while (p < p_end && (p = strstr(p, "%%Page: ")) != NULL) {
        nr++;
        p += strlen("%%Page: ");
    }

    return nr;
}

int epstypesetter_inkspill(const char *tabpath, const tulip_single_note_ctx *song) {
    char *tempfile = tlptemp();
    int has_error = 0;
    char *psdata = NULL;
    FILE *fp = NULL;
    size_t dsize = 0;
    int page_nr = 0;

    pstypesetter_disable_pagenumbering();

    if ((has_error = ps_typesetter(song, tempfile)) != 0) {
        goto ___epstypesetter_inkspill_epilogue;
    }

    psdata = epstypesetter_getpsdata(tempfile, &dsize);
    remove(tempfile);

    if (psdata == NULL) {
        printf("ERROR: empty Postscript output.\n");
        has_error = 1;
        goto ___epstypesetter_inkspill_epilogue;
    }

    if ((page_nr = epstypesetter_get_page_nr(psdata)) != 1) {
        printf("ERROR: the tablature file is not a single page document, for this reason it cannot be exported as EPS (the output has %d pages).\n", page_nr);
        has_error = 1;
        goto ___epstypesetter_inkspill_epilogue;
    }

    if ((fp = fopen(tabpath, "w")) == NULL) {
        printf("ERROR: unable to create \"%s\".\n", tabpath);
        has_error = 1;
        goto ___epstypesetter_inkspill_epilogue;
    }

    epstypesetter_mkheader(fp, pstypesetter_get_carriage_y());

    fwrite(psdata, 1, dsize, fp);

    epstypesetter_mktrailer(fp);

___epstypesetter_inkspill_epilogue:

    pstypesetter_enable_pagenumbering();

    if (psdata != NULL) {
        free(psdata);
    }

    if (fp != NULL) {
        fclose(fp);
    }

    return has_error;
}
