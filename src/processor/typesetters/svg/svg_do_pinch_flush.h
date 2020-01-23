/*
 *                                Copyright (C) 2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_PROCESSOR_TYPESETTERS_SVG_SVG_DO_PINCH_FLUSH_H
#define TULIP_PROCESSOR_TYPESETTERS_SVG_SVG_DO_PINCH_FLUSH_H 1

// WARN(Rafael): Those macros were designed to be used into svginkspill.c only.

// TODO(Rafael): Add some documentation for do_xpack() and do_pinch_flush

#define do_xpack(xspan) {\
    if (last_xstep != NULL) {\
        last_xstep(-1);\
        last_xstep = NULL;\
        *g_svg_page.tab.carriage_x += xspan;\
        svgtypesetter_refresh_fbrd_xy();\
    }\
}

#define do_pinch_flush(xstep, s, sn, arrow_string) {\
    if (*(s) == '~') {\
        do_xpack(7);\
        svgtypesetter_flush_vibrato_pinch();\
        g_svg_page.tab.curr_ln_info->x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.last_symbol = kTlpVibrato;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                xstep(-1);\
            }\
            xstep = svgtypesetter_vibrato_xstep;\
        }\
    } else if (*(s) == 'b') {\
        do_xpack(7);\
        svgtypesetter_flush_bend_pinch(sn == arrow_string);\
        g_svg_page.tab.last_symbol = kTlpBend;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                xstep(-1);\
            }\
            xstep = svgtypesetter_bend_xstep;\
        }\
    } else if (*(s) == 'r') {\
        do_xpack(7);\
        svgtypesetter_flush_release_bend_pinch(sn == arrow_string);\
        g_svg_page.tab.last_symbol = kTlpReleaseBend;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                xstep(-1);\
            }\
            xstep = svgtypesetter_release_bend_xstep;\
        }\
    } else if (*(s) == 'h' || *(s) == 'p') {\
        svgtypesetter_flush_hammer_on_pull_off_pinch();\
        g_svg_page.tab.last_symbol = kTlpHammerOn;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                xstep(-1);\
            }\
            xstep = svgtypesetter_hammer_on_xstep;\
        }\
    } else if (*(s) == '/') {\
        if (last_xstep == svgtypesetter_chord_span_xstep) {\
            last_xstep(-1);\
            do_xpack(13);\
        } else {\
            do_xpack(10);\
        }\
        svgtypesetter_flush_slide_down_pinch();\
        g_svg_page.tab.last_symbol = kTlpSlideDown;\
        g_svg_page.tab.curr_ln_info->x = *g_svg_page.tab.carriage_x;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                xstep(-1);\
            }\
            xstep = svgtypesetter_slide_down_xstep;\
        }\
    } else if (*(s) == '\\') {\
        if (last_xstep == svgtypesetter_chord_span_xstep) {\
            last_xstep(-1);\
            do_xpack(14);\
        } else {\
            do_xpack(11);\
        }\
        svgtypesetter_flush_slide_up_pinch();\
        g_svg_page.tab.last_symbol = kTlpSlideUp;\
        g_svg_page.tab.curr_ln_info->x = *g_svg_page.tab.carriage_x;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                xstep(-1);\
            }\
            xstep = svgtypesetter_slide_up_xstep;\
        }\
    } else if (*(s) == '|' && sn == 5) {\
        svgtypesetter_flush_sep_bar();\
        g_svg_page.tab.last_symbol = kTlpSepBar;\
        g_svg_page.tab.ln_info[0].x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.ln_info[1].x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.ln_info[2].x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.ln_info[3].x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.ln_info[4].x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.ln_info[5].x = *g_svg_page.tab.carriage_x;\
        xstep = svgtypesetter_sep_bar_xstep;\
    } else if (isdigit(*(s)) || *s == 'X' || *s == '?') {\
        if (is_chord && notes_span.do_span && !notes_span.is_beyond_9th_fret[sn]) {\
            *g_svg_page.tab.carriage_x += 7;\
        }\
        svgtypesetter_flush_note_pinch(s);\
        g_svg_page.tab.last_symbol = kTlpSingleNote;\
        if (is_chord && notes_span.do_span && !notes_span.is_beyond_9th_fret[sn]) {\
            *g_svg_page.tab.carriage_x -= 7;\
        }\
        g_svg_page.tab.curr_ln_info->x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.curr_ln_info->do_carriage_return = NULL;\
        xstep = svgtypesetter_note_sep_xstep;\
    }\
}

#endif
