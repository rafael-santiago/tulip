/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/printers/releasebendtxtprinter.h>
#include <processor/typesetters/txt/printers/singletxt.h>

void txttypesetter_releasebend_printer(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note) {
    txttypesetter_single_printer(kTlpReleaseBend, tab, note);
}
