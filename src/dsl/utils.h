/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_DSL_UTILS_H
#define TULIP_DSL_UTILS_H 1

#include <base/types.h>
#include <ctype.h>
#include <stdlib.h>

#define is_technique_block_begin(c) ( (c) == '{' )

#define is_technique_block_end(c) ( (c) == '}' )

#define is_technique_tag_announce(c) ( (c) == '.' )

#define is_sep(c) ( (c) == '-' )

#define is_hammeron(c) ( (c) == 'h' )

#define is_pulloff(c) ( (c) == 'p' )

#define is_vibrato(c) ( (c) == '~' )

#define is_slidedown(c) ( (c) == '/' )

#define is_slideup(c) ( (c) == '\\' )

#define is_note(c) ( ( (c) >= '1' && (c) <= '6' ) )

#define is_string_delim(c) ( (c) == '"' )

#define is_string_escaper(c) ( (c) == '\\' )

#define is_blank(c) ( (c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n' )

#define is_save_point(c) ( (c) == ';' )

#define is_sep_bar(c) ( (c) == '|' )

#define is_bend(c) ( (c) == 'b' )

#define is_release_bend(c) ( (c) == 'r' )

#define is_tapping(c) ( (c) == 'T' )

#define is_natural_harmonic(c) ( (c) == '*' )

#define is_artificial_harmonic(c) ( (c) == 'v' )

#define is_string_jmp(c) ( (c) == ':' )

#define is_muffled(c) ( (c) == 'X' )

#define is_anyfret(c) ( (c) == '?' )

#define is_oncemore(c) ( (c) == '@' )

#define is_sustain(c) ( (c) == '.' )

#define is_note_sep(c) ( is_sep(c) || is_hammeron(c) || is_pulloff(c) || is_vibrato(c) ||\
                         is_slidedown(c) || is_slideup(c) || is_bend(c) || is_release_bend(c) || is_tapping(c) ||\
                         is_natural_harmonic(c) || is_artificial_harmonic(c) )

#define has_sustained_technique(c) ( ( (c) & kTlpMute           ) ||\
                                     ( (c) & kTlpLetRing        ) ||\
                                     ( (c) & kTlpChord          ) ||\
                                     ( (c) & kTlpStrum          ) ||\
                                     ( (c) & kTlpTremoloPicking ) ||\
                                     ( (c) & kTlpVibratoWBar    ) ||\
                                     ( (c) & kTlpTrill          ) )

#define has_non_sustained_technique(c) ( ( (c) & kTlpVibrato )            ||\
                                         ( (c) & kTlpSlideDown )          ||\
                                         ( (c) & kTlpSlideUp )            ||\
                                         ( (c) & kTlpHammerOn )           ||\
                                         ( (c) & kTlpPullOff )            ||\
                                         ( (c) & kTlpBend )               ||\
                                         ( (c) & kTlpReleaseBend )        ||\
                                         ( (c) & kTlpTapping )            ||\
                                         ( (c) & kTlpNaturalHarmonic )    ||\
                                         ( (c) & kTlpArtificialHarmonic ) ||\
                                         ( (c) & kTlpNoteSep ) )

tulip_command_t get_cmd_code_from_cmd_tag(const char *buf);

int is_single_note(const char *buf);

int is_sustained_technique(const char *buf);

size_t tlp_cmd_code_to_plain_index(const tulip_command_t code);

int is_valid_note_cipher(const char *cipher);

tulip_command_t *demux_tlp_commands(const tulip_command_t commands, size_t *array_size);

const char *get_cmd_tag_from_cmd_code(const tulip_command_t cmd);

#endif
