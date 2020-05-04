/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/mobi/mobiinkspill.h>
#include <processor/typesetters/jpeg/jpeg.h>
#include <processor/typesetters/txt/txt.h>
#include <processor/typesetters/txt/txtctx.h>
#include <processor/utils/get_temp_filename.h>
#include <usrland/cmdlineoptions.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

#define MOBITYPESETTER_PAGE_BREAK "<br style=\"page-break-after:always\">"

enum mobitypesetter_filename_indexes {
    kCoverImage = 0,
    kTabImage,
    kCoverHtml,
    kTabHtml,
    kOpf,
    kNcx,
    kFileNamesNr
};

static char *g_mobitypesetter_opf[] = {
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
    "<package xmlns=\"http://www.idpf.org/2007/opf\"\n"
    "         xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
    "         version=\"2.0\">\n"
    "<metadata>\n"
    "        <dc:title>%s</dc:title>\n"
    "        <dc:creator>%s</dc:creator>\n"
    "        <dc:language>%s</dc:language>\n"
    "        <meta name=\"cover\" content=\"cover-image\"/>\n"
    "</metada>\n"
    "<manifest>\n"
    "    <item id=\"cover\" href=\"%s\" media-type=\"application/xhtml+xml\"/>\n"
    "    <item id=\"tab\" href=\"%s\" media-type=\"application/xhtml+xml\"/>\n"
    "    <item id=\"cover-image\" href=\"%s\" media-type=\"image/png\"/>\n"
    "</manifest>\n"
    "<spine toc=\"ncx\">\n"
    "    <itemref idref=\"cover\" linear=\"no\"/>\n"
    "    <itemref idref=\"tab\"/>\n"
    "</spine>\n"
    "<guide>\n"
    "    <reference href=\"%s\" type=\"cover\" title=\"Cover\"/>\n"
    "</guide>\n"
    "</package>\n"
};

static char *g_mobitypesetter_ncx_head[] = {
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
    "<!DOCTYPE ncx PUBLIC \"-//NISO//DTD ncx 2005-1//EN\"\n"
    "                 \"http://www.daisy.org/z3986/2005/ncx-2005-1.dtd\">\n"
    "<ncx xmlns=\"http://www.daisy.org/z3986/2005/ncx/\" version=\"2005-1\">\n"
    "  <head>\n"
    "    <meta name=\"dtb:depth\" content=\"1\"/>\n"
    "    <meta name=\"dtb:totalPageCount\" content=\"0\"/>\n"
    "    <meta name=\"dtb:maxPageNumber\" content=\"0\"/>\n"
    "  </head>\n"
    "  <docTitle>\n"
    "    <text>%s</text>\n"
    "  </docTitle>\n"
    "  <navMap>\n"
};

static char *g_mobitypesetter_ncx_body[] = {
    "    <navPoint id=\"navpoint-%d\" playOrder=\"%d\">\n"
    "      <navLabel>\n"
    "        <text>%s</text>\n"
    "      </navLabel>\n"
    "      <content src=\"%s\"/>\n"
    "    </navPoint>\n"
};

static char *g_mobitypesetter_ncx_tail[] = {
    "  </navMap>\n"
    "</ncx>\n"
};

static int mobitypesetter_create_opf(const char *opf_filename,
                                     const char *title,
                                     const char *creator,
                                     const char *language,
                                     const char *cover_html_filename,
                                     const char *tab_html_filename,
                                     const char *cover_image_filename);

static int mobitypesetter_create_cover_html(const char *cover_filename, const char *cover_image_filename, const char *title);

static int mobitypesetter_create_tab_html(const char *tab_html_filename);

static int mobitypesetter_create_ncx(const char *ncx_filename, const char *title,
                                     const char *cover_html_filename,
                                     const char *tab_html_filename,
                                     const char *language);

static void mobitypesetter_cleanup_tempfiles(const char **filenames, const char *basename);

int mobitypesetter_inkspill(const char *filepath, const tulip_single_note_ctx *song) {
    int has_error = 1;
    char basename[4096], filename[kFileNamesNr][4096];
    txttypesetter_tablature_ctx *tab = NULL;
    const char *language;

    if (filepath == NULL || song == NULL) {
        goto mobitypesetter_inkspill_epilogue;
    }

    if ((tab = txt_typesetter_gettab(song)) == NULL) {
        goto mobitypesetter_inkspill_epilogue;
    }

    if (get_temp_filename(basename, sizeof(basename) - 1, "tmpMobi", 7) == NULL) {
        fprintf(stderr, "ERROR: Unable to get a temporary basename.\n");
        goto mobitypesetter_inkspill_epilogue;
    }

    // INFO(Rafael): Setting up all temp file names.
    snprintf(filename[kTabImage], sizeof(filename[kTabImage]) - 1, "%s.jpeg", basename);
    snprintf(filename[kCoverImage], sizeof(filename[kCoverImage]) - 1, "%s.png", basename);
    snprintf(filename[kCoverHtml], sizeof(filename[kCoverHtml]) - 1, "%s-cover.html", basename);
    snprintf(filename[kTabHtml], sizeof(filename[kTabHtml]) - 1, "%s-tab.html", basename);
    snprintf(filename[kOpf], sizeof(filename[kOpf]) - 1, "%s.opf", basename);
    snprintf(filename[kOpf], sizeof(filename[kNcx]) - 1, "%s.ncx", basename);

    if ((has_error = jpeg_typesetter(song, filename[kTabImage])) == NULL) {
        goto mobitypesetter_inkspill_epilogue;
    }

    language = get_option("mobi-language", NULL);

    if ((has_error = mobitypesetter_create_opf(filename[kOpf],
                                               tab->song,
                                               tab->transcriber,
                                               language,
                                               filename[kCoverHtml],
                                               filename[kTabHtml],
                                               filename[kCoverImage])) != 0) {
        goto mobitypesetter_inkspill_epilogue;
    }

    // TODO(Rafael): Generate cover image.

    if ((has_error = mobitypesetter_create_cover_html(filename[kCoverHtml],
                                                      filename[kCoverImage],
                                                      tab->song)) != 0) {
        goto mobitypesetter_inkspill_epilogue;
    }

    if ((has_error = mobitypesetter_create_tab_html(filename[kTabHtml])) != 0) {
        goto mobitypesetter_inkspill_epilogue;
    }

    if ((has_error = mobitypesetter_create_ncx(filename[kNcx],
                                               tab->song,
                                               filename[kCoverHtml],
                                               filename[kTabHtml],
                                               language)) != 0) {
        goto mobitypesetter_inkspill_epilogue;
    }

    // TODO(Rafael): Generate MOBI.

mobitypesetter_inkspill_epilogue:

    if (tab != NULL) {
        free_txttypesetter_tablature_ctx(tab);
    }

    mobitypesetter_cleanup_tempfiles((char **)filename, basename);

    return has_error;
}

static void mobitypesetter_cleanup_tempfiles(const char **filenames, const char *basename) {
    char **filename, **filename_end;
    struct stat st;
    char temp[4096];
    size_t page_nr;

    filename = filenames;
    filename_end = filenames + kFileNamesNr;

    while (filename != filename_end) {
        if (strstr(*filename, ".jpeg") != NULL) {
            remove(*filename);
        } else {
            page_nr = 1;
            snprintf(temp, sizeof(temp) - 1, "%s-%03d.jpeg", basename, page_nr);
            while (stat(temp, &st) == 0) {
                remove(temp);
                page_nr++;
                snprintf(temp, sizeof(temp) - 1, "%s-%03d.jpeg", basename, page_nr);
            }
        }
        filename++;
    }
}

static int mobitypesetter_create_ncx(const char *ncx_filename, const char *title,
                                     const char *cover_html_filename,
                                     const char *tab_html_filename,
                                     const char *language) {
    FILE *ncx = fopen(ncx_filename, "wb");
    int has_error = 1;
    struct stat st;
    char temp_basename[4096], *tp, page_filename[4096];
    size_t page_nr, p;

    if (ncx == NULL) {
        fprintf(stderr, "ERROR: Unable to create NCX file.\n");
        goto mobitypesetter_create_ncx_epilogue;
    }

    snprintf(temp_basename, sizeof(temp_basename) - 1, "%s", ncx_filename);
    if ((tp = strstr(temp_basename, ".ncx")) == NULL) {
        fprintf(stderr, "ERROR: Unable to guess up the basename from temporary file name : '%s'.\n", ncx_filename);
        goto mobitypesetter_create_ncx_epilogue;
    }

    *tp = 0;

    page_nr = 1;
    snprintf(page_filename, sizeof(page_filename) - 1, "%s-%03d.jpeg", temp_basename, page_nr);

    while (stat(page_filename, &st) == 0) {
        page_nr++;
        snprintf(page_filename, sizeof(page_filename) - 1, "%s-%03d.jpeg", temp_basename, page_nr);
    }

    if (page_nr == 1) {
        fprintf(stderr, "ERROR: There is nothing to output. Your tablature file is empty.\n");
        goto mobitypesetter_create_ncx_epilogue;
    }

    page_nr += 1;

    fprintf(ncx, "%s", g_mobitypesetter_ncx_head, (title != NULL) ? title : "Untitled");

    fprintf(ncx, "%s", g_mobitypesetter_ncx_body, 1, 1, "Cover", cover_html_filename);

    for (p = 2; p < page_nr; p++) {
        fprintf(ncx, "%s", g_mobitypesetter_ncx_body, page_nr, page_nr, "Page", tab_html_filename);
    }

    fprintf(ncx, "%s", g_mobitypesetter_ncx_tail);

    has_error = 0;

mobitypesetter_create_ncx_epilogue:

    if (ncx != NULL) {
        fclose(ncx);
    }

    return has_error;
}

static int mobitypesetter_create_tab_html(const char *tab_html_filename) {
    FILE *html = fopen(tab_html_filename, "wb");
    int has_error = 1;
    struct stat st;
    char page_filename[4096], temp_basename[4096], *tp;
    int page_nr;

    if (html == NULL) {
        fprintf(stderr, "ERROR: Unable to create tab HTML file.\n");
        goto mobitypesetter_create_tab_html_epilogue;
    }

    snprintf(temp_basename, sizeof(temp_basename) - 1, "%s", tab_html_filename);
    if ((tp = strstr(temp_basename, ".html")) == NULL) {
        fprintf(stderr, "ERROR: Unable to guess up the basename from temporary file name : '%s'.\n", tab_html_filename);
        goto mobitypesetter_create_tab_html_epilogue;
    }

    *tp = 0;

    page_nr = 1;
    snprintf(page_filename, sizeof(page_filename) - 1, "%s-%03d.jpeg", temp_basename, page_nr);

    fprintf(html, "<html>\n"
                  "\t<body>\n");

    while (stat(page_filename, &st) == 0) {
        if (page_nr > 1) {
            fprintf(html, "\t\t%s\n", MOBITYPESETTER_PAGE_BREAK);
        }
        fprintf(html, "\t\t<img src=\"%s\">\n", page_filename);
        page_nr++;
        snprintf(page_filename, sizeof(page_filename) - 1, "%s-%03d.jpeg", temp_basename, page_nr);
    }

    if (page_nr == 1) {
        fprintf(stderr, "ERROR: There is nothing to output. Your tablature file is empty.\n");
        goto mobitypesetter_create_tab_html_epilogue;
    }

    fprintf(html, "\t</body>\n"
                  "</html>\n");

    has_error = 0;

mobitypesetter_create_tab_html_epilogue:

    if (html != NULL) {
        fclose(html);
    }

    return has_error;
}

static int mobitypesetter_create_cover_html(const char *cover_filename, const char *cover_image_filename, const char *title) {
    FILE *html = fopen(cover_filename, "wb");
    int has_error = 1;

    if (html == NULL) {
        fprintf(stderr, "ERROR: Unable to create cover HTML file.\n");
        goto mobitypesetter_create_cover_html_epilogue;
    }

    fprintf(html, "<html <html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
                  "\t<head>\n"
                  "\t\t<link type=\"text/css\" rel=\"stylesheet\" href=\"stylesheet.css\"/>\n");

    if (title != NULL) {
        fprintf(html, "\t\t<title>%s</title>\n", title);
    }

    fprintf(html, "\t</head>\n"
                  "\t<body>\n");

    if (cover_image_filename != NULL) {
        fprintf(html, "\t\t<div><img src=\"%s\"></div>\n", cover_image_filename);
    } else {
        fprintf(html, "\t\t<h1>%s</h1>\n", (title != NULL) ? title : "Untitled");
    }

    fprintf(html,"\t</body>\n"
                 "</html>\n");

    has_error = 0;

mobitypesetter_create_cover_html_epilogue:

    if (html != NULL) {
        fclose(html);
    }

    return has_error;
}

static int mobitypesetter_create_opf(const char *opf_filename,
                                     const char *title,
                                     const char *creator,
                                     const char *language,
                                     const char *cover_html_filename,
                                     const char *tab_html_filename,
                                     const char *cover_image_filename) {
    FILE *opf = fopen(opf_filename, "wb");
    int has_error = 1;

    if (opf == NULL) {
        fprintf(stderr, "ERROR: Unable to create opf file.\n");
        goto mobitypesetter_create_opf_epilogue;
    }

    fprintf(opf, "%s", g_mobitypesetter_opf, (title != NULL) ? title : "Untitled",
                                             (creator != NULL) ? creator : "Unknown",
                                             (language != NULL) ? language : "en-US",
                                             cover_html_filename, tab_html_filename, cover_image_filename);

    has_error = 0;

mobitypesetter_create_opf_epilogue:

    if (opf != NULL) {
        fclose(opf);
    }

    return has_error;
}

#undef MOBITYPESETTER_PAGE_BREAK
