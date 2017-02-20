/*
 *                           Copyright (C) 2005-2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSOR_TYPESETTERS_PS_PSINKSPILL_H
#define TULIP_PROCESSOR_TYPESETTERS_PS_PSINKSPILL_H 1

#include <processor/typesetters/txt/txttypes.h>

int pstypesetter_inkspill(const char *filepath, const txttypesetter_tablature_ctx *tab, const tulip_single_note_ctx *song);

void pstypesetter_disable_pagenumbering(void);

void pstypesetter_enable_pagenumbering(void);

int pstypesetter_get_carriage_y(void);

#endif
