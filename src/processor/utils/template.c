/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/utils/template.h>
#include <base/memory.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int apply_action_to_template(char **buffer, size_t *buffer_size,
                             const char *action, const char *value, const size_t value_size) {
    char *bp, *bp_end, *ap, *ap_end;
    char temp[4096];
    char *rp, *rp_end;

    if (buffer == NULL || buffer_size == NULL || *buffer == NULL || *buffer_size == 0 ||
        action == NULL) {
        return 0;
    }

    if (strstr(action, "{{.") != action || strstr(action, "}}") != action + strlen(action) - 2) {
        // INFO(Rafael): Malformed action.
        return 0;
    }

    bp = *buffer;
    bp_end = bp + *buffer_size;

    if ((ap = strstr(bp, action)) == NULL) {
        snprintf(temp, sizeof(temp) - 1, "%s", action);
        ap_end = strstr(temp, "}}");
        ap_end[0] = ':';
        ap_end[1] = 0;
        ap = strstr(bp, temp);
    }

    if (ap != NULL) {
        ap_end = ap;
        while (ap_end != bp_end && *ap_end != '}') {
            ap_end++;
        }

        if (ap_end == bp_end) {
            // INFO(Rafael): Unterminated action.
            return 0;
        }

        while (ap_end != bp_end && *ap_end == '}') {
            ap_end++;
        }

        if (ap_end == bp_end && ap_end[-1] != '}') {
            // INFO(Rafael): Unterminated action.
            return 0;
        }

        rp = (char *) getseg(*buffer_size + value_size);
        memset(rp, 0, *buffer_size + value_size);
        rp_end = rp;

        memcpy(rp_end, bp, ap - bp);
        rp_end += ap - bp;
        memcpy(rp_end, value, value_size);
        rp_end += value_size;
        if (ap_end < bp_end) {
            memcpy(rp_end, ap_end, bp_end - ap_end);
            rp_end += bp_end - ap_end;
        }

        *buffer_size = rp_end - rp;
        rp = (char *) realloc(rp, *buffer_size);

        if (rp == NULL) {
            fprintf(stderr, "tulip PANIC: no memory!\n");
            exit(1);
        }

        free(bp);
        *buffer = rp;

        return 1 + apply_action_to_template(buffer, buffer_size, action, value, value_size);
    }

    return 0;
}

char *get_action_default_value(const char *template_buffer, const size_t template_buffer_size,
                               const char *action, size_t *value_size) {
    char *value = NULL;
    const char *tp_end, *ap, *ap_end;
    char temp[4096], *tp;
    size_t v_size;

    if (value_size != NULL) {
        *value_size = 0;
    }

    if (template_buffer == NULL || template_buffer_size == 0 || action == NULL) {
        goto get_action_default_value_epilogue;
    }

    if (strstr(action, "{{.") != action || strstr(action, "}}") != action + strlen(action) - 2) {
        //INFO(Rafael): Malformed action.
        goto get_action_default_value_epilogue;
    }

    snprintf(temp, sizeof(temp) - 1, "%s", action);
    tp = strstr(temp, "}}");
    tp[0] = ':';
    tp[1] = 0;

    if ((ap = strstr(template_buffer, temp)) == NULL) {
        goto get_action_default_value_epilogue;
    }

    tp_end = template_buffer + template_buffer_size;
    ap += strlen(temp);
    ap_end = ap;

    while (ap_end != tp_end && *ap_end != '}') {
        ap_end++;
    }

    if (ap_end == tp_end) {
        // INFO(Rafael): Malformed template.
        goto get_action_default_value_epilogue;
    }

    v_size = ap_end - ap;

    value = getseg(v_size + 1);
    memset(value, 0, v_size + 1);
    memcpy(value, ap, v_size);

    if (value_size != NULL) {
        *value_size = v_size;
    }

get_action_default_value_epilogue:

    return value;
}
