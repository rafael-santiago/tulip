/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <usrland/usropt2tlpopt.h>
#include <base/memory.h>
#include <string.h>

static struct usropt2tlpopt_ctx *get_tlpopt(const char *option, const char *entire_buf);

struct get_tlpopt_callvect {
    const char *option;
    struct usropt2tlp_opt_ctx *(*get)(const char *option, const char *entire_buf);
};

#define register_new_tlpopt_callout(o, c) { (o), (c) }

static struct get_tlpopt_callvect g_tlpopt_callvect[] = {
    register_new_tlpopt_callout("fretboard-size", NULL),
    register_new_tlpopt_callout("fretboard-style", NULL),
    register_new_tlpopt_callout("close-tab-to-save", NULL),
    register_new_tlpopt_callout("indentation-deepness", NULL),
    register_new_tlpopt_callout("include-tab-notation", NULL),
    register_new_tlpopt_callout("cut-tab-on-last-note", NULL),
    register_new_tlpopt_callout("add-tunning-to-the-fretboard", NULL),
};

static const g_tlpopt_callvect_nr = sizeof(g_tlpopt_callvect) / sizeof(g_tlpopt_callvect[0]);

static struct usropt2tlpopt_ctx *get_tlpopt(const char *option, const char *entire_buf) {
    return NULL;
}

void free_usropt2tlpopt_ctx(struct usropt2tlpopt_ctx *p) {
    if (p == NULL) {
        return;
    }
    free(p->option);
    free(p->data);
    free(p);
}

struct usropt2tlpopt_ctx *usropt2tlpopt(const char *data) {
    char option[255], *op = NULL;
    const char *dp = NULL, *dp_end = NULL;
    if (data == NULL) {
        return NULL;
    }
    dp = data;
    dp_end = dp + strlen(data);
    while (dp != dp_end && *dp != '=') {
        dp++;
    }
    memset(option, 0, sizeof(option));
    memcpy(option, data, dp_end - dp - (*dp == '='));
    return get_tlpopt(option, data);
}
