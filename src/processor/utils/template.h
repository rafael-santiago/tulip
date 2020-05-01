/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSOR_UTILS_TEMPLATE_H
#define TULIP_PROCESSOR_UTILS_TEMPLATE_H 1

#include <stdlib.h>

char *get_action_default_value(const char *template_buffer, const size_t template_buffer_size,
                               const char *action, size_t *value_size);

int apply_action_to_template(char **buffer, size_t *buffer_size,
                             const char *action, const char *value, const size_t value_size);

#endif
