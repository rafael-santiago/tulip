/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <system/version.h>

static const char *g_tulip_system_version = "v6";

const char *get_tulip_system_version() {
    return g_tulip_system_version;
}
