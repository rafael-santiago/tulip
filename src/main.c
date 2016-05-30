/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <system/init.h>
#include <system/exec.h>

int main(int argc, char **argv) {
    tulip_system_init(argc, argv);
    return tulip_task_exec();
}
