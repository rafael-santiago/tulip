/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/mobi/mobi.h>

#define MOBITYPESETTER_PAGE_BREAK "<br style=\"page-break-after:always\">"

static char *g_mobi_typesetter_opf[] = {
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
    "<package xmlns=\"http://www.idpf.org/2007/opf\"\n"
    "         xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
    "         version=\"2.0\">\n"
    "<metadata>\n"
    "        <dc:title>%s</dc:title>\n"
    "        <dc:creator>%s</dc:creator>\n"
    "        <dc:language>pt-BR</dc:language>\n"
    "        <meta content=\"%s\" name=\"cover-image\"/>\n"
    "</metada>\n"
    "<manifest>\n"
    "    <item id=\"cover\" href=\"%s\" media-type=\"application/xhtml+xml\"/>\n"
    "    <item id=\"tabs\" href=\"%s\" media-type=\"application/xhtml+xml\"/>\n"
    "    <item id=\"cover-image\" href=\"%s\" media-type=\"image/png\"/>\n"
    "</manifest>\n"
    "<spine toc=\"ncx\">\n"
    "    <itemref idref=\"cover\" linear=\"no\"/>\n"
    "    <itemref idref=\"tabs\"/>\n"
    "</spine>\n"
    "<guide>\n"
    "    <reference href=\"%s\" type=\"cover\" title=\"Cover\"/>\n"
    "</guide>\n"
    "</package>\n"
};

int mobi_typesetter(const tulip_single_note_ctx *song, const char *tabpath) {
    return 1;
}

#undef MOBITYPESETTER_PAGE_BREAK
