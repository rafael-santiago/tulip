#ifndef TULIP_BASE_TYPES_H
#define TULIP_BASE_TYPES_H 1

typedef enum _tulip_command_t {
    kTlpNone               = 0x00000000,
    kTlpMute               = 0x00000001,
    kTlpLetRing            = 0x00000002,
    kTlpChord              = 0x00000004,
    kTlpBeat               = 0x00000008,
    kTlpTremoloPicking     = 0x00000010,
    kTlpVibrato            = 0x00000020,
    kTlpSlideDown          = 0x00000040,
    kTlpSlideUp            = 0x00000080,
    kTlpHammerOn           = 0x00000100,
    kTlpPullOff            = 0x00000200,
    kTlpVibratoWBar        = 0x00000400,
    kTlpTunning            = 0x00000800,
    kTlpLiteral            = 0x00001000,
    kTlpSingleNote         = 0x00002000,
    kTlpNoteSep            = 0x00004000,
    kTlpSepBar             = 0x00008000,
    kTlpSavePoint          = 0x00010000,
    kTlpBend               = 0x00020000,
    kTlpReleaseBend        = 0x00040000,
    kTlpBlockEnd           = 0x00080000,
    kTlpTapping            = 0x00100000,
    kTlpNaturalHarmonic    = 0x00200000,
    kTlpArtificialHarmonic = 0x00400000
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
    kTlpPrefsCutTabToSave   = 0x0000000000000001,
    kTlpPrefsCloseTabToSave = 0x0000000000000020,
    kTlpPrefsShowTunning    = 0x0000000000000040
}tulip_prefs_map;

typedef struct _tulip_preferences_ctx {
    unsigned int fretboard_size;
    tulip_prefs_map prefs;
}tulip_preferences_ctx;

typedef struct _tulip_code_ctx {
    tulip_preferences_ctx *preferences;
    char *head, *footer;
    tulip_single_note_ctx *notes;
}tulip_code_ctx;

#endif
