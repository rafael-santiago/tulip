/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSOR_TYPESETTERS_SVG_SVGBOUNDARIES_H
#define TULIP_PROCESSOR_TYPESETTERS_SVG_SVGBOUNDARIES_H 1

// INFO(Rafael): A4 paper w x h dimension.
#define SVGTYPESETTER_DEFAULT_PAGE_WIDTH        595
#define SVGTYPESETTER_DEFAULT_PAGE_HEIGHT       842

#define SVGTYPESETTER_TAB_XL_DELTA               10
#define SVGTYPESETTER_TAB_XR_DELTA              -05

#define SVGTYPESETTER_TAB_Y                      50
#define SVGTYPESETTER_TAB_Y_SPAN                 10

#define SVGTYPESETTER_TAB_X_SPAN                 15

#define SVGTYPESETTER_TAB_NOTE_Y_OFFSET          05

#define SVGTYPESETTER_CHORD_SPAN_X_STEP          07

#define SVGTYPESETTER_USER_NOTE_SPAN_X_STEP      05

#define SVGTYPESETTER_FRETBOARD_SIZE             80

//INFO(Rafael): It is totally heuristic and/or based on
//              my own preferences.

#define SVGTYPESETTER_BEND_MIN_SPACE                07
#define SVGTYPESETTER_RELEASE_BEND_MIN_SPACE        07
#define SVGTYPESETTER_VIBRATO_MIN_SPACE             07
#define SVGTYPESETTER_SLIDE_DOWN_MIN_SPACE          07
#define SVGTYPESETTER_SLIDE_UP_MIN_SPACE            07
#define SVGTYPESETTER_NATURAL_HARMONIC_MIN_SPACE    07
#define SVGTYPESETTER_ARTIFICIAL_HARMONIC_MIN_SPACE 07
#define SVGTYPESETTER_TAPPING_MIN_SPACE             07

#define SVGTYPESETTER_CHORD_NOTES_PADDING        07

#define SVGTYPESETTER_DEFAULT_SVG_ENCODING       "ISO-8859-1"

#define SVGTYPESETTER_DEFAULT_SVG_EMBED_FONT     0

#define svgtypesetter_is_off_xmargin(x) ((x) >= (g_svgtypesetter_page_width - SVGTYPESETTER_TAB_X_SPAN - 10))

extern int g_svgtypesetter_page_width;

extern int g_svgtypesetter_page_height;

extern char *g_svgtypesetter_svg_encoding;

extern int g_svgtypesetter_svg_embed_font;

#endif
