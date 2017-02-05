/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/typeprefs.h>
#include <processor/settings.h>

static struct typesetter_curr_settings g_typesetter_settings = { 0 };

struct typesetter_curr_settings  typesetter_settings() {
    void *data = NULL;

    if (g_typesetter_settings.prefs == 0) {

        data = get_processor_setting("indentation-deepness", NULL);

        g_typesetter_settings.indentation_deepness = ( data == NULL) ? 0 : *(size_t *)data;

        data = get_processor_setting("prefs", NULL);
        if (data != NULL) {
            g_typesetter_settings.prefs = *(tulip_prefs_map_t *)data;
        }
    }

    return g_typesetter_settings;
}
