/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/typeprefs.h>
#include <processor/settings.h>
#include <usrland/cmdlineoptions.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

struct typesetter_paper_info_ctx {
    char *name;
    int width, height;
};

static struct typesetter_curr_settings g_typesetter_settings = { 0 };

static struct typesetter_paper_info_ctx g_typesetter_papers[] = {
    { "4A0", 4768, 6741 },
    { "2A0", 3370, 4768 },
    {  "A0", 2384, 3370 },
    {  "A1", 1684, 2384 },
    {  "A2", 1191, 1684 },
    {  "A3",  842, 1191 },
    {  "A4",  595,  842 },
    {  "A5",  420,  595 },
    {  "A6",  298,  420 },
    {  "A7",  210,  298 },
    {  "A8",  147,  210 },
    {  "A9",  105,  147 },
    {  "A10",  74,  105 }
};

static int g_typesetter_papers_nr = sizeof(g_typesetter_papers) / sizeof(g_typesetter_papers[0]);

static struct typesetter_paper_info_ctx *typesetter_get_paper_info(void);

struct typesetter_curr_settings  typesetter_settings() {
    void *data = NULL;

    data = get_processor_setting("indentation-deepness", NULL);

    g_typesetter_settings.indentation_deepness = (data == NULL) ? 0 : *(size_t *)data;

    data = get_processor_setting("prefs", NULL);
    if (data != NULL) {
        g_typesetter_settings.prefs = *(tulip_prefs_map_t *)data;
    }

    return g_typesetter_settings;
}

int typesetter_paper_size(int *width, int *height) {
    int no_error = 0;
    struct typesetter_paper_info_ctx *p_info;
    const char *option;

    if (width == NULL || height == NULL) {
        goto typesetter_paper_size_epilogue;
    }

    *width = 0;
    *height = 0;

    if ((p_info = typesetter_get_paper_info()) != NULL) {
        *width = p_info->width;
        *height = p_info->height;
    } else if ((option = get_option("paper-width", NULL)) != NULL) {
        *width = atoi(option);
        if (*width > 0 && (option = get_option("paper-height", NULL)) != NULL) {
            *height = atoi(option);
        }
    }

    no_error = (*width > 0 && *height > 0);

typesetter_paper_size_epilogue:

    return no_error;
}

static struct typesetter_paper_info_ctx *typesetter_get_paper_info(void) {
    char *paper, *p, *p_end;
    struct typesetter_paper_info_ctx *p_info, *p_info_end;

    if ((paper = (char *)get_option("paper", NULL)) != NULL) {

        p = paper;
        p = p + strlen(p);

        while (p != p_end) {
            *p = toupper(*p);
            p++;
        }

        p_info = &g_typesetter_papers[0];
        p_info_end = p_info + g_typesetter_papers_nr;

        while (p_info != p_info_end) {
            if (strcmp(paper, p_info->name) == 0) {
                return p_info;
            }
        }
    }

    return NULL;
}
