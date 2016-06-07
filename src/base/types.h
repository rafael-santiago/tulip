/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_BASE_TYPES_H
#define TULIP_BASE_TYPES_H 1

typedef enum _tulip_command_t {
    kTlpNone               = 0x00000000,
    kTlpSavePoint          = 0x00000001,
    kTlpNoteSep            = 0x00000002,
    kTlpBlockEnd           = 0x00000004,
    kTlpMute               = 0x00000008,
    kTlpLetRing            = 0x00000010,
    kTlpChord              = 0x00000020,
    kTlpStrum              = 0x00000040,
    kTlpTremoloPicking     = 0x00000080,
    kTlpVibratoWBar        = 0x00000100,
    kTlpTimes              = 0x00000200,
    kTlpVibrato            = 0x00000400,
    kTlpSlideDown          = 0x00000800,
    kTlpSlideUp            = 0x00001000,
    kTlpHammerOn           = 0x00002000,
    kTlpPullOff            = 0x00004000,
    kTlpTunning            = 0x00008000,
    kTlpLiteral            = 0x00010000,
    kTlpSingleNote         = 0x00020000,
    kTlpSepBar             = 0x00040000,
    kTlpBend               = 0x00080000,
    kTlpReleaseBend        = 0x00100000,
    kTlpTapping            = 0x00200000,
    kTlpNaturalHarmonic    = 0x00400000,
    kTlpArtificialHarmonic = 0x00800000,
    kTlpOnceMore           = 0x01000000
}tulip_command_t;

typedef struct _tulip_technique_stack_ctx {
    tulip_command_t technique_code;
    struct _tulip_technique_stack_ctx *next;
}tulip_technique_stack_ctx;

typedef struct _tulip_single_note_ctx {
    tulip_command_t techniques;
    char buf[255];
    int line_nr;
    struct _tulip_single_note_ctx *last, *next;
}tulip_single_note_ctx;

typedef enum _tulip_bool_prefs_map {
    kTlpPrefsCutTabOnTheLastNote      = 0x0000000000000001,
    kTlpPrefsCloseTabToSave           = 0x0000000000000020,
    kTlpPrefsShowTunning              = 0x0000000000000040,
    kTlpPrefsFretboardStyleNormal     = 0x0000000000000080,
    kTlpPrefsFretboardStyleContinuous = 0x0000000000000100,
    kTlpPrefsIncludeTabNotation       = 0x0000000000000200
}tulip_prefs_map_t;

#endif
