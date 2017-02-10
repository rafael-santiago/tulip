/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/settings.h>
#include <base/types.h>
#include <string.h>

struct processor_settings_ctx {
    tulip_prefs_map_t prefs;
    size_t fretboard_size;
    char tunning[6][3];
    size_t indentation_deepness;
};

struct processor_setting_handler_ctx {
    const char *setting;
    void (*set)(const void *data, const size_t dsize);
    void *(*get)(size_t *dsize);
};

static void set_fretboard_size(const void *data, const size_t dsize);

static void *get_fretboard_size(size_t *dsize);

static void set_tunning(const void *data, const size_t dsize);

static void *get_tunning(size_t *dsize);

static void set_indentation_deepness(const void *data, const size_t dsize);

static void *get_indentation_deepness(size_t *dsize);

static void set_tulip_bitmap(const void *data, const size_t dsize);

static void *get_tulip_bitmap(size_t *dsize);

#define register_new_processor_setting(s, ss, g) { (s), (ss), (g) }

static struct processor_setting_handler_ctx g_processor_setting_handlers[] = {
    register_new_processor_setting("prefs", set_tulip_bitmap, get_tulip_bitmap),
    register_new_processor_setting("tunning", set_tunning, get_tunning),
    register_new_processor_setting("fretboard-size", set_fretboard_size, get_fretboard_size),
    register_new_processor_setting("indentation-deepness", set_indentation_deepness, get_indentation_deepness)
};

const size_t g_processor_setting_handlers_nr = sizeof(g_processor_setting_handlers) / sizeof(g_processor_setting_handlers[0]);

#define DEFAULT_TULIP_PROCESSOR_SETTINGS { kTlpPrefsCutTabOnTheLastNote   |\
                                           kTlpPrefsCloseTabToSave        |\
                                           kTlpPrefsFretboardStyleNormal, 80, { "e", "B", "G", "D", "A", "E"}, 8 }

static struct processor_settings_ctx g_processor_settings = DEFAULT_TULIP_PROCESSOR_SETTINGS;

static struct processor_setting_handler_ctx *get_setting_handler(const char *setting);

static struct processor_setting_handler_ctx *get_setting_handler(const char *setting) {
    size_t p = 0;
    if (setting == NULL) {
        return NULL;
    }
    for (p = 0; p < g_processor_setting_handlers_nr; p++) {
        if (strcmp(g_processor_setting_handlers[p].setting, setting) == 0) {
            return (&g_processor_setting_handlers[p]);
        }
    }
    return NULL;
}

void set_processor_setting(const char *setting, const void *data, const size_t dsize) {
    struct processor_setting_handler_ctx *handler = NULL;

    if (setting == NULL && data == NULL) {
        return;
    }

    handler = get_setting_handler(setting);

    if (handler != NULL && handler->set != NULL) {
        handler->set(data, dsize);
    }

}

void *get_processor_setting(const char *setting, size_t *dsize) {
    void *data = NULL;
    struct processor_setting_handler_ctx *handler = NULL;
    if (setting == NULL) {
        return NULL;
    }
    handler = get_setting_handler(setting);
    return (handler != NULL && handler->get != NULL) ? handler->get(dsize) : NULL;
}

static void set_fretboard_size(const void *data, const size_t dsize) {
    if (data == NULL) {
        return;
    }
    g_processor_settings.fretboard_size = *(size_t *)data;
}

static void *get_fretboard_size(size_t *dsize) {
    if (dsize != NULL) {
        *dsize = sizeof(g_processor_settings.fretboard_size);
    }
    return &g_processor_settings.fretboard_size;
}

static void set_tunning(const void *data, const size_t dsize) {
    const char *buf = NULL;
    const char *bp = NULL;
    const char *bp_end = NULL;
    size_t t = 0, s = 5;
    int has_half_step_notes = 0;

    if (data == NULL) {
        return;
    }

    buf = (char *)data;
    bp = buf;
    bp_end = bp + strlen(bp);

    while (bp != bp_end && s >= 0) {
        if (*bp == '-' || (bp + 1) == bp_end) {
            if ((bp + 1) == bp_end) {
                g_processor_settings.tunning[s][t] = *bp;
                t = (t + 1) % sizeof(g_processor_settings.tunning[0]);
            }
            g_processor_settings.tunning[s][t] = 0;
            if (strlen(g_processor_settings.tunning[s]) == 2) {
                has_half_step_notes = 1;
            }
            s--;
            t = 0;
        } else {
            g_processor_settings.tunning[s][t] = *bp;
            t = (t + 1) % sizeof(g_processor_settings.tunning[0]);
        }
        bp++;
    }

    if (has_half_step_notes) {
        s = 0;
        while (s < 6) {
            if (strlen(g_processor_settings.tunning[s]) == 1) {
                strncat(g_processor_settings.tunning[s], " ", sizeof(g_processor_settings.tunning[s]) - 1);
            }
            s++;
        }
    }
}

static void *get_tunning(size_t *dsize) {
    static char *tp[6] = {
        g_processor_settings.tunning[0],
        g_processor_settings.tunning[1],
        g_processor_settings.tunning[2],
        g_processor_settings.tunning[3],
        g_processor_settings.tunning[4],
        g_processor_settings.tunning[5]
    };
    if (dsize != NULL) {
        *dsize = sizeof(g_processor_settings.tunning) / sizeof(g_processor_settings.tunning[0]);
    }
    return tp;
}

static void set_indentation_deepness(const void *data, const size_t dsize) {
    if (data == NULL) {
        return;
    }
    g_processor_settings.indentation_deepness = *(size_t *)data;
}

static void *get_indentation_deepness(size_t *dsize) {
    if (dsize != NULL) {
        *dsize = sizeof(g_processor_settings.indentation_deepness);
    }
    return &g_processor_settings.indentation_deepness;
}

static void set_tulip_bitmap(const void *data, const size_t dsize) {
    g_processor_settings.prefs = *(tulip_prefs_map_t *)data;
}

static void *get_tulip_bitmap(size_t *dsize) {
    if (dsize != NULL) {
        *dsize = sizeof(g_processor_settings.prefs);
    }
    return &g_processor_settings.prefs;
}
