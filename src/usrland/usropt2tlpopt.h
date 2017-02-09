/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_USRLAND_USROPT2TLPOPT_H
#define TULIP_USRLAND_USROPT2TLPOPT_H 1

#include <base/types.h>
#include <stdlib.h>

struct usropt2tlpopt_ctx {
    char *option;
    void *data;
    size_t dsize;
};

void free_usropt2tlpopt_ctx(struct usropt2tlpopt_ctx *p);

struct usropt2tlpopt_ctx *usropt2tlpopt(const char *data);

tulip_prefs_map_t get_prefs_mask(const char *option, const char *data);

#endif
