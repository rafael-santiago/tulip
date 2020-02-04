/*
 *                           Copyright (C) 2005-2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <encoding/html_str_normalize.h>
#include <string.h>

#define is_forbidden_char(c) ( (c) == '&' || (c) == '>' || (c) == '<' )

static char *g_html_entities[256];

static size_t g_html_entities_init_done = 0;

static inline void html_entities_init(void);

static inline void html_entities_init(void) {
    memset(g_html_entities, 0, sizeof(g_html_entities));
    g_html_entities[0x26] = "&amp;";
    g_html_entities[0x3C] = "&lt;";
    g_html_entities[0x3E] = "&gt;";
    g_html_entities_init_done = 1;
}

char *html_str_normalize(char *buf, size_t buf_size, const char *str) {
    // INFO(Rafael): Replace some symbols to their HTML entity correspondent.
    const char *sp, *sp_end;
    char *bp, *bp_end, *ep;
    size_t e_size;

    if (buf == NULL || buf_size == 0) {
        return buf;
    }

    memset(buf, 0, buf_size);

    if (str == NULL) {
        return buf;
    }

    if (!g_html_entities_init_done) {
        html_entities_init();
    }

    bp = buf;
    bp_end = bp + buf_size - 1;

    sp = str;
    sp_end = sp + strlen(sp);

    while (bp < bp_end && sp != sp_end) {
        if (!is_forbidden_char(*sp)) {
            *bp = *sp;
            bp++;
        } else {
            ep = g_html_entities[(size_t)*sp];
            e_size = (ep != NULL) ? strlen(ep) : 0;
            if (e_size > 0 && (bp + e_size) < bp_end) {
                memcpy(bp, ep, e_size);
            }
            bp += e_size;
        }
        sp++;
    }

    return buf;
}

#undef is_forbidden_char
