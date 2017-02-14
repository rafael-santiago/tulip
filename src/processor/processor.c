/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/processor.h>
#include <processor/typesetters/txt/txt.h>
#include <processor/typesetters/ps/ps.h>
#include <processor/typesetters/eps/eps.h>
#include <processor/typesetters/pdf/pdf.h>
#include <processor/typesetters/md/md.h>
#include <processor/typesetters/html/html.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define register_tulip_processors typedef int (*typesetter_callout)(const tulip_single_note_ctx *song, const char *tabpath);\
                                  struct tlp_processor_ctx {\
                                    const char *ext;\
                                    typesetter_callout typesetter;\
                                  };\
                                  static struct tlp_processor_ctx g_processors[] = {

#define register_new_tlp_processor(e) { "."#e, e ##_typesetter }

#define register_tulip_processors_end { NULL, blackhole_processor } };\
                                         static const size_t g_processors_nr = sizeof(g_processors) / sizeof(g_processors[0]);

static int blackhole_processor(const tulip_single_note_ctx *song, const char *tabpath);

//static int coming_soon(const tulip_single_note_ctx *song, const char *tabpath);

register_tulip_processors
    register_new_tlp_processor(txt),
    register_new_tlp_processor(pdf),
    register_new_tlp_processor(ps),
    register_new_tlp_processor(eps),
    register_new_tlp_processor(md),
    register_new_tlp_processor(html),
register_tulip_processors_end

/*
static int coming_soon(const tulip_single_note_ctx *song, const char *tabpath) {
    printf("WARNING: the processor to produce the file \"%s\" is not totally implemented yet! coming soon!\n", tabpath);
    return 1;
}
*/

int blackhole_processor(const tulip_single_note_ctx *song, const char *tabpath) {
    printf("processor ERROR: Tulip has no processor capable of producing the file \"%s\".\n", tabpath);
    return 1;
}

int mktab(const tulip_single_note_ctx *song, const char *tabpath) {
    size_t p = 0;
    const char *tp = NULL;

    if (tabpath == NULL) {
        //  WARN(Santiago): Let's put the guilty little-devel ashamed ;)
        printf("processor PANIC: Hey Beavis! Huh!! We got a (null) @tabpath here!!"
               " Yeah, yeah!! Butt-Head it must not happen... Huh-Huh! Yeah..."
               " This is pretty... ... NULL! Huh-Huh.. Yeah Hehuhehehuhehhehuhhuhuhehu "
               "null he-he he-he! Huh-Huh!\n");
        return 1;
    }

    tp = tabpath + strlen(tabpath) - 1;

    while (tp != tabpath && *tp != '.') {
        tp--;
    }

    if (*tp != '.') {
        printf("processor ERROR: Unspecified output extension.\n");
        return 1;
    }

    for (p = 0; p < g_processors_nr; p++) {
        if (g_processors[p].ext == NULL) {
            continue;
        }
        if (strcmp(g_processors[p].ext, tp) == 0) {
            return g_processors[p].typesetter(song, tabpath);
        }
    }

    return g_processors[g_processors_nr - 1].typesetter(NULL, tabpath);
}
