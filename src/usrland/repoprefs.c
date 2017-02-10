/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <usrland/repoprefs.h>
#include <usrland/usropt2tlpopt.h>
#include <base/types.h>
#include <base/memory.h>
#include <processor/settings.h>
#include <stdio.h>
#include <string.h>

void ld_repo_prefs() {
    FILE *fp = fopen(".tulipprefs", "rb");
    char line[255] = "", *lp = NULL, *lp_end = NULL;
    char *file_buffer = NULL, *fb = NULL, *fb_end = NULL;
    long file_size = 0;
    struct usropt2tlpopt_ctx *setting = NULL;
    tulip_prefs_map_t prefs = 0;
    void *data = NULL;
    size_t dsize = 0;
    if (fp == NULL) {
        return;
    }
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    file_buffer = getseg(file_size + 1);
    memset(file_buffer, 0, file_size + 1);
    fb = file_buffer;
    fb_end = fb + file_size;
    fseek(fp, 0L, SEEK_SET);
    fread(file_buffer, 1, file_size, fp);
    fclose(fp);
    lp = &line[0];
    lp_end = lp + sizeof(line);
    while (fb != fb_end) {
        if ((*fb == '\n' || *fb == '\r' || fb + 1 == fb_end) && lp != &line[0]) {
            if (fb + 1 == fb_end && *fb != '\n' && *fb != '\r') {
                *lp = *fb;
                lp++;
            }
            *lp = 0;
            lp = &line[0];
            setting = usropt2tlpopt(lp);
            if (setting == NULL) {
                continue;
            }
            if (strcmp(setting->option, "prefs") != 0) {
                set_processor_setting(setting->option, setting->data, setting->dsize);
            } else {
                //printf("%s (%s %.8X)\n", line, setting->option, *(tulip_prefs_map_t *)setting->data);
                data = get_processor_setting("prefs", &dsize);
                lp = strstr(line, "=");
                if (lp != NULL) {
                    lp++;
                }
                if (strcmp(lp, "no")    == 0 ||
                    strcmp(lp, "0")     == 0 ||
                    strcmp(lp, "false") == 0) {
                    prefs = (*(tulip_prefs_map_t *)data) & (*(tulip_prefs_map_t *)setting->data);
                } else {
                    prefs = (*(tulip_prefs_map_t *)data) | (*(tulip_prefs_map_t *)setting->data);
                }
                data = &prefs;
                dsize = sizeof(prefs);
                set_processor_setting("prefs", data, dsize);
                lp = &line[0];
            }
            free_usropt2tlpopt_ctx(setting);
        } else {
            if (lp < lp_end && *fb != '\n' && *fb != '\r') {
                *lp = *fb;
                lp++;
            }
        }
        fb++;
    }
    free(file_buffer);
}
