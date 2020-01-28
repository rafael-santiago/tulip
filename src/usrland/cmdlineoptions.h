/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_USRLAND_CMDLINEOPTIONS_H
#define TULIP_USRLAND_CMDLINEOPTIONS_H 1

void set_option_argc_argv(const int argc, char **argv);

const char *get_option(const char *option, const char *default_value);

int get_bool_option(const char *option, const int default_value);

const char *get_binary_path(void);

#endif
