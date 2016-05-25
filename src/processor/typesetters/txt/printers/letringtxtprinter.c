/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/printers/letringtxtprinter.h>
#include <processor/typesetters/txt/printers/sustainedtxt.h>

void txttypesetter_letring_printer(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note) {
    txttypesetter_sustained_printer(kTlpLetRing, tab, note);
}
