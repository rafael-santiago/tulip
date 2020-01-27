/*
 *                           Copyright (C) 2005-2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <system/init.h>
#include <processor/settings.h>
#include <usrland/cmdlineoptions.h>
#include <usrland/repoprefs.h>
#include <usrland/usropt2tlpopt.h>
#include <base/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void tulip_system_init(int argc, char **argv) {
    const char *value = NULL;
    char *overwritten_options[] = {
        "fretboard-size",
        "fretboard-style",
        "close-tab-to-save",
        "indentation-deepness",
        "include-tab-notation",
        "cut-tab-on-the-last-note",
        "add-tuning-to-the-fretboard",
        "show-tuning"
    };
    const size_t overwritten_options_nr = sizeof(overwritten_options) / sizeof(overwritten_options[0]);
    size_t o = 0;
    char usroption[255] = "";
    struct usropt2tlpopt_ctx *setting = NULL;
    tulip_prefs_map_t prefs = 0;
    void *data = NULL;
    size_t dsize = 0;

    ld_repo_prefs();

    set_option_argc_argv(argc, argv);

    for (o = 0; o < overwritten_options_nr; o++) {
        value = get_option(overwritten_options[o], NULL);
        if (value != NULL) {
            sprintf(usroption, "%s=%s", overwritten_options[o], value);
            setting = usropt2tlpopt(usroption);
            if (setting == NULL) {
                continue;
            }
            if (strcmp(setting->option, "prefs") != 0) {
                set_processor_setting(setting->option, setting->data, setting->dsize);
            } else {
                data = get_processor_setting("prefs", &dsize);

                if (strcmp(value, "no")    == 0  ||
                    strcmp(value,  "0")    == 0  ||
                    strcmp(value, "false") == 0) {
                    prefs = *(tulip_prefs_map_t *)data & *(tulip_prefs_map_t *)setting->data;
                } else {
                    prefs = *(tulip_prefs_map_t *)data | *(tulip_prefs_map_t *)setting->data;
                }

                data = &prefs;
                dsize = sizeof(prefs);

                set_processor_setting("prefs", data, dsize);
            }
            free_usropt2tlpopt_ctx(setting);
        }
    }
}
