/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/processor.h>
#include <processor/typesetters/txt/txt.h>
#include <stdlib.h>

struct tlp_processor_ctx {
    const char *ext;
    int (*typesetter)(const tulip_single_note_ctx *song, const char *tabpath);
};

#define register_new_tlp_processor(e, t) { (e), (t) }

static struct tlp_processor_ctx g_processors[] = {
    register_new_tlp_processor("txt", txt_typesetter)
};

int mktab(const tulip_single_note_ctx *song, const char *tabpath) {
    return 0;
}
