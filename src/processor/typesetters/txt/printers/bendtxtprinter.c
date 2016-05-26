#include <processor/typesetters/txt/printers/bendtxtprinter.h>
#include <processor/typesetters/txt/printers/singletxt.h>

void txttypesetter_bend_printer(txttypesetter_tablature_ctx **tab, const tulip_single_note_ctx *note) {
    txttypesetter_single_printer(kTlpBend, tab, note);
}
