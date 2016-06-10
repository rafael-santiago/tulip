/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <usrland/usropt2tlpopt.h>
#include <base/memory.h>
#include <base/types.h>
#include <dsl/compiler/compiler.h>
#include <string.h>
#include <stdio.h>

static struct usropt2tlpopt_ctx *get_tlpopt(const char *option, const char *entire_buf);

static struct usropt2tlpopt_ctx *get_size_t(const char *option, const char *entire_buf);

static struct usropt2tlpopt_ctx *get_tulip_prefs_map_t(const char *option, const char *entire_buf);

static struct usropt2tlpopt_ctx *get_fretboard_style_t(const char *option, const char *entire_buf);

static struct usropt2tlpopt_ctx *get_tunning(const char *option, const char *entire_buf);

struct get_tlpopt_callvect {
    const char *option;
    struct usropt2tlpopt_ctx *(*get)(const char *option, const char *entire_buf);
};

#define register_new_tlpopt_callout(o, c) { (o), (c) }

static struct get_tlpopt_callvect g_tlpopt_callvect[] = {
    register_new_tlpopt_callout("fretboard-size", get_size_t),
    register_new_tlpopt_callout("fretboard-style", get_fretboard_style_t),
    register_new_tlpopt_callout("close-tab-to-save", get_tulip_prefs_map_t),
    register_new_tlpopt_callout("indentation-deepness", get_size_t),
    register_new_tlpopt_callout("include-tab-notation", get_tulip_prefs_map_t),
    register_new_tlpopt_callout("cut-tab-on-the-last-note", get_tulip_prefs_map_t),
    register_new_tlpopt_callout("add-tunning-to-the-fretboard", get_tulip_prefs_map_t),
    register_new_tlpopt_callout("tunning", get_tunning),
};

static const g_tlpopt_callvect_nr = sizeof(g_tlpopt_callvect) / sizeof(g_tlpopt_callvect[0]);

static struct usropt2tlpopt_ctx *get_tlpopt(const char *option, const char *entire_buf) {
    const char *op = option;
    size_t c = 0;

    if (op == NULL || entire_buf == NULL) {
        return NULL;
    }

    while (*op == '-' && *op != 0) {
        op++;
    }

    for (c = 0; c < g_tlpopt_callvect_nr; c++) {
        if (strcmp(op, g_tlpopt_callvect[c].option) == 0) {
            return g_tlpopt_callvect[c].get(op, entire_buf);
        }
    }

    return NULL;
}

static struct usropt2tlpopt_ctx *get_tunning(const char *option, const char *entire_buf) {
    struct usropt2tlpopt_ctx *opt = NULL;
    size_t t = 0;
    char buf[255] = "";
    const char *bp = NULL;
    tulip_single_note_ctx *tunning = NULL;
    int is_ok = 0;

    if (option == NULL || entire_buf == NULL) {
        return NULL;
    }

    sprintf(buf, ".tunning{%s}", entire_buf);
    is_ok = compile_tulip_codebuf(buf, NULL, &tunning, &bp);
    if (!is_ok) {
        return;
    }
    free_tulip_single_note_ctx(tunning);

    opt = (struct usropt2tlpopt_ctx *)getseg(sizeof(struct usropt2tlpopt_ctx));

    opt->option = (char *)getseg((t = strlen(option) + 1));
    memset(opt->option, 0, t);
    memcpy(opt->option, option, t);

    opt->dsize = strlen(entire_buf);
    opt->data = getseg(opt->dsize + 1);
    memset(opt->data, 0, opt->dsize + 1);
    memcpy(opt->data, entire_buf, opt->dsize);

    return opt;
}

static struct usropt2tlpopt_ctx *get_size_t(const char *option, const char *entire_buf) {
    const char *p = NULL;
    char buf[255] = "", *bp = NULL, *bp_end = NULL;
    struct usropt2tlpopt_ctx *opt = NULL;
    size_t t = 0;
    if (option == NULL || entire_buf == NULL) {
        return NULL;
    }
    p = entire_buf;
    if (*p == '=') {
        p++;
    }
    bp = &buf[0];
    bp_end = bp + sizeof(buf);
    while (isdigit(*p) && bp != bp_end) {
        *bp = *p;
        bp++;
        p++;
    }
    *bp = 0;

    opt = (struct usropt2tlpopt_ctx *)getseg(sizeof(struct usropt2tlpopt_ctx));

    opt->option = (char *)getseg((t = strlen(option) + 1));
    memset(opt->option, 0, t);
    memcpy(opt->option, option, t);

    opt->data = getseg(sizeof(size_t));
    opt->dsize = sizeof(size_t);

    *(size_t *)opt->data = atoi(buf);

    return opt;
}


static struct usropt2tlpopt_ctx *get_tulip_prefs_map_t(const char *option, const char *entire_buf) {
    char *bitmap_options[] = {
        "cut-tab-on-the-last-note",
        "close-tab-to-save",
        "add-tunning-to-the-fretboard",
        NULL,
        NULL,
        "include-tab-notation"
    };
    const size_t bitmap_options_nr = sizeof(bitmap_options) / sizeof(bitmap_options[0]);
    size_t b;
    tulip_prefs_map_t pref = 1;
    struct usropt2tlpopt_ctx *opt = NULL;

    if (option == NULL || entire_buf == NULL) {
        return;
    }

    for (b = 0; b < bitmap_options_nr; b++) {
        if (bitmap_options[b] == NULL) {
            continue;
        }
        if (strcmp(bitmap_options[b], option) == 0) {
            break;
        }
    }

    if (strcmp(entire_buf, "1")    == 0  ||
        strcmp(entire_buf, "yes")  == 0  ||
        strcmp(entire_buf, "true") == 0  ||
        strcmp(entire_buf, "on")   == 0) {
        pref = pref << b;
    } else if (strcmp(entire_buf, "0")     == 0 ||
               strcmp(entire_buf, "no")    == 0 ||
               strcmp(entire_buf, "false") == 0 ||
               strcmp(entire_buf, "off")   == 0) {
        pref = ~(pref << b);
    }

    opt = (struct usropt2tlpopt_ctx *)getseg(sizeof(struct usropt2tlpopt_ctx));

    opt->option = (char *)getseg(32);
    memset(opt->option, 0, 32);
    strncpy(opt->option, "prefs", 31);

    opt->data = getseg(sizeof(tulip_prefs_map_t));
    opt->dsize = sizeof(tulip_prefs_map_t);

    *(tulip_prefs_map_t *)opt->data = pref;

    return opt;
}

static struct usropt2tlpopt_ctx *get_fretboard_style_t(const char *option, const char *entire_buf) {
    struct usropt2tlpopt_ctx *opt = NULL;
    tulip_prefs_map_t pref = 0;

    if (entire_buf == NULL || option == NULL) {
        return NULL;
    }

    if (strcmp(option, "fretboard-style") != 0) {
        return NULL;
    }

    if (strstr(entire_buf, "normal") == entire_buf) {
        pref = kTlpPrefsFretboardStyleNormal | (~kTlpPrefsFretboardStyleContinuous);
    } else if (strstr(entire_buf, "continuous") == entire_buf) {
        pref = kTlpPrefsFretboardStyleContinuous | (~kTlpPrefsFretboardStyleNormal);
    }

    opt = (struct usropt2tlpopt_ctx *)getseg(sizeof(struct usropt2tlpopt_ctx));

    opt->option = (char *)getseg(32);
    strncpy(opt->option, "prefs", 31);

    opt->data = getseg(sizeof(tulip_prefs_map_t));
    opt->dsize = sizeof(tulip_prefs_map_t);

    *(tulip_prefs_map_t *)opt->data = pref;

    return opt;
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
    memcpy(option, data, dp - data);
    return get_tlpopt(option, dp + (*dp == '='));
}
