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

// INFO(Rafael): The macro do_xpack() performs a direct carriage return based on the last non-null pinch, e.g. a pinch
//               different from '-'. After decreasing x carriage, it moves the x carriage for xspan positions; do_xpack
//               is only called indirectly, through the macro do_pinch_flush.

#define do_xpack(xspan) {\
    if (last_xstep != NULL) {\
        last_xstep(-1);\
        last_xstep = NULL;\
        *g_svg_page.tab.carriage_x += xspan;\
        svgtypesetter_refresh_fbrd_xy();\
    }\
}

// INFO(Rafael): The macro do_pinch_flush() is where the major 'incantations' of the SVG typesetter happen.
//               Based on the current non-null separator (a separator not equals to '-'), it adjusts the
//               carriage x of the current string and also pinches the separator representation on SVG tab diagram
//               of the current SVG stream. After adjusting and pinching, it schedules an increment for
//               the carriage x by setting xstep to a proper related 'technique_xstep' function. However, notice
//               that this x axis increment will only occur after traversing all six strings of the current TAB section.
//               In this way, the xstep setting only happens when xstep is equals to NULL, svgtypesetter_note_sep_xstep or
//               svgtypesetter_user_note_span_xstep.
//
//               - svgtypesetter_note_sep_xstep stands for the single space of a single '-'. E.g.: '30-32'
//               - svgtypesetter_user_note_span_xstep stands for a sequence of single '-' inserted by the user, as a way of
//                 getting more space between last and current note(s). E.g.: '.letring{60-52-42-31-20-10-------}|'
//
//               This is the general idea behind do_pinch_flush, any other specific code chunk is directly commented.

#define do_pinch_flush(xstep, s, sn, arrow_string, is_chord) {\
    if (*(s) == '~') {\
        do_xpack(SVGTYPESETTER_VIBRATO_MIN_SPACE);\
        svgtypesetter_flush_vibrato_pinch();\
        g_svg_page.flush_nr++;\
        /*INFO(Rafael): Setting the ln_info[s].x to the exact position where the technique was drawn.*/\
        g_svg_page.tab.curr_ln_info->x = *g_svg_page.tab.carriage_x;\
        /*INFO(Rafael): Recording which symbol was last processed is quite important.*/\
        g_svg_page.tab.last_symbol = kTlpVibrato;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                /*INFO(Rafael): Once found the end of the user's space filling, let's undo the last xstep and set*/\
                /*              xstep to technique xstep.*/\
                xstep(-1);\
            }\
            xstep = svgtypesetter_vibrato_xstep;\
        }\
    } else if (*(s) == 'b') {\
        do_xpack(SVGTYPESETTER_BEND_MIN_SPACE);\
        svgtypesetter_flush_bend_pinch(sn == arrow_string);\
        g_svg_page.flush_nr++;\
        g_svg_page.tab.curr_ln_info->x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.last_symbol = kTlpBend;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                xstep(-1);\
            }\
            xstep = svgtypesetter_bend_xstep;\
        }\
    } else if (*(s) == 'r') {\
        do_xpack(SVGTYPESETTER_RELEASE_BEND_MIN_SPACE);\
        svgtypesetter_flush_release_bend_pinch(sn == arrow_string);\
        g_svg_page.flush_nr++;\
        g_svg_page.tab.curr_ln_info->x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.last_symbol = kTlpReleaseBend;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                xstep(-1);\
            }\
            xstep = svgtypesetter_release_bend_xstep;\
        }\
    } else if (*(s) == 'h' || *(s) == 'p') {\
        svgtypesetter_flush_hammer_on_pull_off_pinch();\
        g_svg_page.flush_nr++;\
        g_svg_page.tab.last_symbol = kTlpHammerOn;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                xstep(-1);\
            }\
            xstep = svgtypesetter_hammer_on_xstep;\
        }\
    } else if (*(s) == '/') {\
        if (last_xstep == svgtypesetter_chord_span_xstep) {\
            /*INFO(Rafael): When this separator is wrapped inside a chord there is a xstep excess side-effect. It is being*/\
            /*              compensated here.*/\
            last_xstep(-1);\
            do_xpack(SVGTYPESETTER_SLIDE_DOWN_MIN_SPACE + SVGTYPESETTER_SLIDE_DOWN_MIN_SPACE - 1);\
        } else {\
            do_xpack(SVGTYPESETTER_SLIDE_DOWN_MIN_SPACE + 3);\
        }\
        svgtypesetter_flush_slide_down_pinch();\
        g_svg_page.flush_nr++;\
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
            /*INFO(Rafael): When this separator is wrapped inside a chord there is a xstep excess side-effect. It is being*/\
            /*              compensated here.*/\
            last_xstep(-1);\
            do_xpack(SVGTYPESETTER_SLIDE_UP_MIN_SPACE + SVGTYPESETTER_SLIDE_UP_MIN_SPACE);\
        } else {\
            do_xpack(SVGTYPESETTER_SLIDE_UP_MIN_SPACE + 4);\
        }\
        svgtypesetter_flush_slide_up_pinch();\
        g_svg_page.flush_nr++;\
        g_svg_page.tab.last_symbol = kTlpSlideUp;\
        g_svg_page.tab.curr_ln_info->x = *g_svg_page.tab.carriage_x;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                xstep(-1);\
            }\
            xstep = svgtypesetter_slide_up_xstep;\
        }\
    } else if (*(s) == '*') {\
        do_xpack(SVGTYPESETTER_NATURAL_HARMONIC_MIN_SPACE);\
        svgtypesetter_flush_natural_harmonic_pinch();\
        g_svg_page.flush_nr++;\
        g_svg_page.tab.curr_ln_info->x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.last_symbol = kTlpNaturalHarmonic;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                xstep(-1);\
            }\
            xstep = svgtypesetter_natural_harmonic_xstep;\
        }\
    } else if (*(s) == 'v') {\
        do_xpack(SVGTYPESETTER_ARTIFICIAL_HARMONIC_MIN_SPACE);\
        svgtypesetter_flush_artificial_harmonic_pinch();\
        g_svg_page.flush_nr++;\
        g_svg_page.tab.curr_ln_info->x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.last_symbol = kTlpArtificialHarmonic;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                xstep(-1);\
            }\
            xstep = svgtypesetter_artificial_harmonic_xstep;\
        }\
    } else if (*(s) == 'T') {\
        do_xpack(SVGTYPESETTER_TAPPING_MIN_SPACE);\
        svgtypesetter_flush_tapping_pinch();\
        g_svg_page.flush_nr++;\
        g_svg_page.tab.curr_ln_info->x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.last_symbol = kTlpTapping;\
        if (xstep == NULL || (xstep == svgtypesetter_note_sep_xstep || xstep == svgtypesetter_user_note_span_xstep)) {\
            if (xstep == svgtypesetter_user_note_span_xstep) {\
                xstep(-1);\
            }\
            xstep = svgtypesetter_tapping_xstep;\
        }\
    } else if (*(s) == '|' && sn == 5) {\
        if (g_svg_page.flush_nr > 0) {\
            /*INFO(Rafael): It will avoid clumsy sep bars heading the TAB diagram.*/\
            svgtypesetter_flush_sep_bar();\
            g_svg_page.flush_nr++;\
        }\
        g_svg_page.tab.last_symbol = kTlpSepBar;\
        /*INFO(Rafael): If the TAB diagram was broke with a sep bar, from now on all strings x coordinates must be the same.*/\
        g_svg_page.tab.ln_info[0].x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.ln_info[1].x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.ln_info[2].x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.ln_info[3].x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.ln_info[4].x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.ln_info[5].x = *g_svg_page.tab.carriage_x;\
        xstep = svgtypesetter_sep_bar_xstep;\
    } else if (isdigit(*(s)) || *s == 'X' || *s == '?') {\
        if (is_chord && notes_span.do_span && !notes_span.is_beyond_9th_fret[sn]) {\
            /*INFO(Rafael): It will send notes below 10th fret to more right, resulting an facier typesetting.*/\
            *g_svg_page.tab.carriage_x += SVGTYPESETTER_CHORD_NOTES_PADDING;\
        }\
        svgtypesetter_flush_note_pinch(s);\
        g_svg_page.flush_nr++;\
        g_svg_page.tab.last_symbol = kTlpSingleNote;\
        if (is_chord && notes_span.do_span && !notes_span.is_beyond_9th_fret[sn]) {\
            /*INFO(Rafael): Moving it back again, thus, notes from 10th or higher will not be screwed-up.*/\
            *g_svg_page.tab.carriage_x -= SVGTYPESETTER_CHORD_NOTES_PADDING;\
        }\
        g_svg_page.tab.curr_ln_info->x = *g_svg_page.tab.carriage_x;\
        g_svg_page.tab.curr_ln_info->do_carriage_return = NULL;\
        xstep = svgtypesetter_note_sep_xstep;\
        if (!is_chord) {\
            /*INFO(Rafael): Since it is not a chord any further return must be done until this current x.*/\
            g_svg_page.tab.ln_info[0].x = *g_svg_page.tab.carriage_x;\
            g_svg_page.tab.ln_info[1].x = *g_svg_page.tab.carriage_x;\
            g_svg_page.tab.ln_info[2].x = *g_svg_page.tab.carriage_x;\
            g_svg_page.tab.ln_info[3].x = *g_svg_page.tab.carriage_x;\
            g_svg_page.tab.ln_info[4].x = *g_svg_page.tab.carriage_x;\
            g_svg_page.tab.ln_info[5].x = *g_svg_page.tab.carriage_x;\
        }\
    }\
}

#endif
