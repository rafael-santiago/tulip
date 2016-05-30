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
    char tunning[6][2];
    size_t indentation_deepness;
};

struct processor_setting_handler_ctx {
    const char *setting;
    void (*set)(const void *data, const size_t dsize);
    void *(*get)(size_t *dsize);
};

#define register_new_processor_setting(s, ss, g) { (s), (ss), (g) }

static struct processor_setting_handler_ctx g_processor_setting_handlers[] = {
    register_new_processor_setting("fretboard-size", NULL, NULL),
    register_new_processor_setting("fretboard-style", NULL, NULL),
    register_new_processor_setting("close-tab-to-save", NULL, NULL),
    register_new_processor_setting("indentation-deepness", NULL, NULL),
    register_new_processor_setting("include-tab-notation", NULL, NULL),
    register_new_processor_setting("cut-tab-on-last-note", NULL, NULL),
    register_new_processor_setting("add-tunning-to-the-fretboard", NULL, NULL),
    register_new_processor_setting("tunning", NULL, NULL)
};

const size_t g_processor_setting_handlers_nr = sizeof(g_processor_setting_handlers) / sizeof(g_processor_setting_handlers[0]);

#define DEFAULT_TULIP_PROCESSOR_SETTINGS { kTlpPrefsCutTabOnTheLastNote   |\
                                           kTlpPrefsCloseTabToSave        |\
                                           kTlpPrefsFretboardStyleNormal, 80, {"e", "B", "G", "D", "A", "E"}, 8 }

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
        return;
    }
    handler = get_setting_handler(setting);
    return (handler != NULL && handler->get != NULL) ? handler->get(dsize) : NULL;
}
