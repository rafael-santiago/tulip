/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <cutest.h>
#include <base/types.h>
#include <base/ctx.h>
#include <dsl/utils.h>
#include <dsl/str/strutils.h>
#include <dsl/parser/parser.h>
#include <dsl/compiler/compiler.h>
#include <processor/oututils.h>
#include <usrland/cmdlineoptions.h>
#include <system/version.h>
#include <system/init.h>
#include <system/exec.h>
#include <processor/typesetters/txt/txtctx.h>
#include <processor/processor.h>
#include "fancytesting.h"
#include <string.h>
#include <stdlib.h>

CUTE_TEST_CASE(base_tulip_technique_stack_ctx_tests)
    tulip_technique_stack_ctx *stack = NULL, *top = NULL;
    stack = push_technique_to_technique_stack_ctx(stack, kTlpChord);
    stack = push_technique_to_technique_stack_ctx(stack, kTlpMute);
    stack = push_technique_to_technique_stack_ctx(stack, kTlpTremoloPicking);
    CUTE_ASSERT(is_empty_technique_stack_ctx(stack) == 0);
    CUTE_ASSERT(top_of_technique_stack_ctx(stack) == kTlpTremoloPicking);
    stack = pop_technique_from_technique_stack_ctx(stack);
    CUTE_ASSERT(top_of_technique_stack_ctx(stack) == kTlpMute);
    stack = pop_technique_from_technique_stack_ctx(stack);
    CUTE_ASSERT(top_of_technique_stack_ctx(stack) == kTlpChord);
    stack = pop_technique_from_technique_stack_ctx(stack);
    CUTE_ASSERT(is_empty_technique_stack_ctx(stack) == 1);
    CUTE_ASSERT(top_of_technique_stack_ctx(stack) == kTlpNone);
    free_technique_stack_ctx(stack);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_basic_dsl_utils_tests)
    //  INFO(Santiago): it tests the basic aspects of the DSL.
    struct expected_results {
        const char *tech;
        tulip_command_t cmd;
        int sustain;
    };
    struct expected_results e_results[] = {
        {           ".mute",               kTlpMute, 1 },
        {        ".letring",            kTlpLetRing, 1 },
        {          ".chord",              kTlpChord, 1 },
        {           ".strum",             kTlpStrum, 1 },
        { ".tremolopicking",     kTlpTremoloPicking, 1 },
        {    ".vibratowbar",        kTlpVibratoWBar, 1 },
        {               "~",            kTlpVibrato, 0 },
        {               "/",          kTlpSlideDown, 0 },
        {              "\\",            kTlpSlideUp, 0 },
        {               "h",           kTlpHammerOn, 0 },
        {               "p",            kTlpPullOff, 0 },
        {        ".tunning",            kTlpTunning, 0 },
        {        ".literal",            kTlpLiteral, 0 },
        {              "34",         kTlpSingleNote, 0 },
        {               "-",            kTlpNoteSep, 0 },
        {               ";",          kTlpSavePoint, 0 },
        {               "|",             kTlpSepBar, 0 },
        {               "b",               kTlpBend, 0 },
        {               "r",        kTlpReleaseBend, 0 },
        {               "T",            kTlpTapping, 0 },
        {               "*",    kTlpNaturalHarmonic, 0 },
        {               "v", kTlpArtificialHarmonic, 0 },
        {          "(null)",               kTlpNone, 0 }
    };
    size_t e_results_nr = sizeof(e_results) / sizeof(e_results[0]), e = 0;
    CUTE_ASSERT(is_technique_block_begin('{') == 1);
    CUTE_ASSERT(is_technique_block_end('}') == 1);
    CUTE_ASSERT(is_technique_tag_announce('.') == 1);
    CUTE_ASSERT(is_sep('-') == 1);
    CUTE_ASSERT(is_hammeron('h') == 1);
    CUTE_ASSERT(is_pulloff('p') == 1);
    CUTE_ASSERT(is_vibrato('~') == 1);
    CUTE_ASSERT(is_slidedown('/') == 1);
    CUTE_ASSERT(is_slideup('\\') == 1);
    CUTE_ASSERT(is_note('1') && is_note('2') && is_note('3') && is_note('4') && is_note('5') && is_note('6') == 1);
    CUTE_ASSERT(is_note('0') == 0);
    CUTE_ASSERT(is_string_delim('"') == 1);
    CUTE_ASSERT(is_string_escaper('\\') == 1);
    CUTE_ASSERT(is_single_note("01") == 0);
    CUTE_ASSERT(is_single_note("10") == 1);
    CUTE_ASSERT(is_single_note("100") == 1);
    CUTE_ASSERT(is_single_note("52") == 1);
    CUTE_ASSERT(is_single_note("502") == 1);
    CUTE_ASSERT(is_single_note("45") == 1);
    CUTE_ASSERT(is_single_note("405") == 1);
    CUTE_ASSERT(is_single_note("37") == 1);
    CUTE_ASSERT(is_single_note("309") == 1);
    CUTE_ASSERT(is_single_note("25") == 1);
    CUTE_ASSERT(is_single_note("206") == 1);
    CUTE_ASSERT(is_single_note("1X") == 1);
    CUTE_ASSERT(is_single_note("2X") == 1);
    CUTE_ASSERT(is_single_note("3X") == 1);
    CUTE_ASSERT(is_single_note("4X") == 1);
    CUTE_ASSERT(is_single_note("5X") == 1);
    CUTE_ASSERT(is_single_note("6X") == 1);
    CUTE_ASSERT(is_single_note("1:h") == 1);
    CUTE_ASSERT(is_single_note("2:-") == 1);
    CUTE_ASSERT(is_single_note("3:b") == 1);
    CUTE_ASSERT(is_single_note("4:r") == 1);
    CUTE_ASSERT(is_single_note("5:p") == 1);
    CUTE_ASSERT(is_single_note("6:*") == 1);
    CUTE_ASSERT(is_single_note("33b") == 1);
    CUTE_ASSERT(is_single_note("3b") == 0);
    for (e = 0; e < e_results_nr; e++) {
        CUTE_ASSERT(get_cmd_code_from_cmd_tag(e_results[e].tech) == e_results[e].cmd);
        CUTE_ASSERT(is_sustained_technique(e_results[e].tech) == e_results[e].sustain);
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_strutils_tests)
    char *string = NULL;
    CUTE_ASSERT(is_valid_string("") == 0);
    CUTE_ASSERT(is_valid_string("\"\n\n\nall ok.\n\n\n\"") == 1);
    CUTE_ASSERT(is_valid_string("\"Hey Beavis, I'm an \"outlaw string, huh.\"") == 0);
    CUTE_ASSERT(is_valid_string("\"Hey Beavis, now I'm a \\\"by the book\\\" string, huh.\"") == 1);
    CUTE_ASSERT(expand_string(NULL) == NULL);
    string = expand_string("\"\t\n\r\".\"");
    CUTE_ASSERT(string != NULL && strlen(string) == 5 && string[0] == '\t' &&
                                                         string[1] == '\n' &&
                                                         string[2] == '\r' &&
                                                         string[3] == '"'  &&
                                                         string[4] == '.'  &&
                                                         string[5] == 0);
    free(string);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_parser_skip_string_chunk_tests)
    const char *string = "\"This is my \\\"string\\\"\".";
    const char *sp = skip_string_chunk(string);
    CUTE_ASSERT(sp != NULL && strcmp(sp, ".") == 0);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_parser_get_next_tlp_technique_block_begin_tests)
    const char *buf = ".chord{30-20-10}";
    const char *exp_res = buf + 6;
    const char *begin = get_next_tlp_technique_block_begin(buf);
    CUTE_ASSERT(begin != NULL && begin == exp_res);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_parser_get_next_tlp_technique_block_end_tests)
    const char *buf = "{.chord{57-49}}";
    const char *buf_end = buf + strlen(buf) - 1;
    const char *end = get_next_tlp_technique_block_end(buf);
    CUTE_ASSERT(end != NULL && end == buf_end);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_parser_get_next_tlp_technique_block_size_tests)
    const char *codebuf = ".mute{.chord{57-49}}";
    size_t expected_size = 14;
    CUTE_ASSERT(get_next_tlp_technique_block_size(codebuf) == expected_size);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_parser_get_codebuf_from_filepath_tests)
    const char *codebuf = "302-200-100-;-.chord{501-400-201-202}";
    char *ldbuf = NULL;
    FILE *fp = fopen("test.tlp", "wb");
    CUTE_ASSERT(fp != NULL);
    fwrite(codebuf, 1, strlen(codebuf), fp);
    fclose(fp);
    ldbuf = get_codebuf_from_filepath("test.tlp");
    remove("test.tlp");
    CUTE_ASSERT(ldbuf != NULL);
    CUTE_ASSERT(strcmp(ldbuf, codebuf) == 0);
    free(ldbuf);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_parser_get_next_tlp_command_tests)
    const char *next = NULL;
    next = get_next_tlp_command("200-301~-200h202~");
    CUTE_ASSERT(next != NULL && *next == '2');
    next = get_next_tlp_command("  -301~-200h202~");
    CUTE_ASSERT(next != NULL && *next == '-');
    next = get_next_tlp_command("~-200h202~");
    CUTE_ASSERT(next != NULL && *next == '~');
    next = get_next_tlp_command("h202~");
    CUTE_ASSERT(next != NULL && *next == 'h');
    next = get_next_tlp_command("  ~");
    CUTE_ASSERT(next != NULL && *next == '~');
    next = get_next_tlp_command("  .letring{.chord{200-202}}");
    CUTE_ASSERT(next != NULL && *next == '.');
    next = get_next_tlp_command(" ;");
    CUTE_ASSERT(next != NULL && *next == ';');
    next = get_next_tlp_command("  p200");
    CUTE_ASSERT(next != NULL && *next == 'p');
    next = get_next_tlp_command("  \\63");
    CUTE_ASSERT(next != NULL && *next == '\\');
    next = get_next_tlp_command("  /605");
    CUTE_ASSERT(next != NULL && *next == '/');
    next = get_next_tlp_command("  |-200");
    CUTE_ASSERT(next != NULL && *next == '|');
    next = get_next_tlp_command("  b-200");
    CUTE_ASSERT(next != NULL && *next == 'b');
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_parser_set_curr_code_line_number_tests)
    CUTE_ASSERT(get_curr_code_line_number() == 0);
    set_curr_code_line_number(255);
    CUTE_ASSERT(get_curr_code_line_number() == 255);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_parser_get_curr_code_line_number_tests)
        const char *codebuf = "\n\n.chord{10-20-30}";
        const char *next = NULL;
        set_curr_code_line_number(1);
        next = get_next_tlp_command(codebuf);
        CUTE_ASSERT(next != NULL);
        CUTE_ASSERT(get_curr_code_line_number() == 3);
        codebuf = "\n\n\"\n\n\\n\"-";
        set_curr_code_line_number(1);
        next = get_next_tlp_command(codebuf);
        CUTE_ASSERT(get_curr_code_line_number() == 5);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(base_tulip_single_note_ctx_tests)
    tulip_single_note_ctx *song = NULL;
    song = add_note_to_tulip_single_note_ctx(song, kTlpChord, "53-42-30-21-10");
    CUTE_ASSERT(song != NULL);
    CUTE_ASSERT(song->next == NULL);
    CUTE_ASSERT(song->last == NULL);
    CUTE_ASSERT(song->techniques == kTlpChord);
    CUTE_ASSERT(strcmp(song->buf, "53-42-30-21-10") == 0);
    song = add_note_to_tulip_single_note_ctx(song, kTlpChord | kTlpLetRing, "53-42-30-21-10");
    CUTE_ASSERT(song->next != NULL);
    CUTE_ASSERT(song->last == NULL);
    CUTE_ASSERT(song->next->next == NULL);
    CUTE_ASSERT(song->next->last == song);
    CUTE_ASSERT(song->next->techniques == kTlpChord | kTlpLetRing);
    CUTE_ASSERT(strcmp(song->next->buf, "53-42-30-21-10") == 0);
    free_tulip_single_note_ctx(song);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_utils_tlp_cmd_code_to_plain_index_tests)
    struct test_vector {
        tulip_command_t code;
        size_t index;
    };
    struct test_vector values[] = {
        {          kTlpSavePoint,  0 },
        {            kTlpNoteSep,  1 },
        {           kTlpBlockEnd,  2 },
        {               kTlpMute,  3 },
        {            kTlpLetRing,  4 },
        {              kTlpChord,  5 },
        {              kTlpStrum,  6 },
        {     kTlpTremoloPicking,  7 },
        {        kTlpVibratoWBar,  8 },
        {              kTlpTrill,  9 },
        {              kTlpTimes, 10 },
        {            kTlpVibrato, 11 },
        {          kTlpSlideDown, 12 },
        {            kTlpSlideUp, 13 },
        {           kTlpHammerOn, 14 },
        {            kTlpPullOff, 15 },
        {            kTlpTunning, 16 },
        {            kTlpLiteral, 17 },
        {         kTlpSingleNote, 18 },
        {             kTlpSepBar, 19 },
        {               kTlpBend, 20 },
        {        kTlpReleaseBend, 21 },
        {            kTlpTapping, 22 },
        {    kTlpNaturalHarmonic, 23 },
        { kTlpArtificialHarmonic, 24 }
    };
    size_t values_nr = sizeof(values) / sizeof(values[0]), v;
    for (v = 0; v < values_nr; v++) {
        CUTE_ASSERT(tlp_cmd_code_to_plain_index(values[v].code) == values[v].index);
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_compiler_compile_tulip_codebuf)
    struct tlpcode_test_vector {
        const char *codebuf;
        int valid;
    };
    struct tlpcode_test_vector tlpcodes[] = {
        //  INFO(Santiago): Toy productions.
        { ".chord{60-52-42-31-20-10}", 1 },
        { ".chord 60-52-42-31-20-10}", 0 },
        { "30", 1 },
        { "3a", 0 },
        { "h", 1 },
        { "h3q", 0 },
        { "p", 1 },
        { "p4#", 0 },
        { "-", 1 },
        { "-3a", 0 },
        { "|", 1 },
        { "|3B", 0 },
        { "-|-", 1 },
        { ";", 1 },
        { ";-3a", 0 },
        { "b", 1 },
        { "b33b", 1 },
        { "b3b", 0 },
        { "r", 1 },
        { "r33r", 1 },
        { "r4r", 0 },
        { "*", 1 },
        { "*3a", 0 },
        { "T", 1 },
        { "T3T", 0 },
        { "v", 1 },
        { "v3a", 0 },
        { "~", 1 },
        { "~3a", 0 },
        { "~4~", 0 },
        { ".mute { 20 - 30 - 40 - 20 }", 1 },
        { ".letring{.chord{20-10}}", 1 },
        { ".strum{.chord{60-52-42-31-20-10}}", 1 },
        { ".tremolopicking{60-------------}", 1 },
        { ".vibratowbar{37****************}", 1 },
        { ".literal{\"boo.\"}", 1 },
        { ".mute { 20 - 30 - 40 - 20 ", 0 },
        { ".letring{.chord{20-10}", 0 },
        { ".strum{.chord{60-52-42-31-20-10}", 0 },
        { ".tremolopicking{60-------------", 0 },
        { ".vibratowbar{37****************", 0 },
        { ".literal{\"boo.\"", 0 },
        //  INFO(Santiago): Real world productions. All following must compile without any error.
        { ".chord{40-30}-.chord{43-33}-.chord{45-35}.chord{4:~3:~}-|-.chord{40-30}-.chord{43-33}-.chord{46-36}.chord{4:p3:p}.chord{45-35}.chord{4:~3:~}-|-.chord{40-30}-.chord{43-33}-.chord{45-35}-.chord{43-33}-.chord{40-30}-;", 1 },
        { ".letring{55-47-37-27---27-37-47-55-47-27--54-46-36-26--46-36-26--24h26h27-14--55-47-37-27--47-37-27--55-27-14b15r--54-46-36-26--46-36-26--26b27r};", 1 },
        { ".chord{600-500-402}--.chord{601-501-403}--.chord{602-502-404}-|;", 1 },
        { "6?/67-55-56-57-57-45b-57-|-6?/67-55-56-57-57-45b-57-|-5?/57-45-46-47-47-35b-47-|-5?/57-45-46-47-47-35b-47-|-6?/67-55-56-57-57-45b-57-|-6?/67-55-56-57-57-45b-57-|-57/59-47-48-.letring{.chord{49-48}-.chord{49-48}-.chord{200-100}-.chord{49-48}-.chord{49-48}-.chord{200-100}}-|-.chord{59-48-39-200}-.chord{500-49-300-201}-.letring{.chord{60-57---}}-|-.chord{200-17}.chord{2:b1:b}-.chord{200-17}.chord{2:b1:b}-.chord{102-202}-205-202-302-404-302b-404-502h504-502h504-;", 1 },
        { "4?/402-.chord{400-300-200}-|-302-200-100-203b204-200-302-300-402-.chord{402-302}-", 1 },
        { ".letring{61-.chord{21-11}-|-63-.chord{23-13}-|-64-.chord{24-14}-|-63-.chord{23-13}-|-51h53-43-}|-", 1 },
        { ".chord{37-100}.chord{3:/1:/}.chord{38-101}-.chord{38-101}-.chord{301-202}-.chord{39-29}-301-300-39-.chord{39-29-101}-.chord{19-29-39}-;", 1 },
        { ".chord{56-46}-.chord{56-46}-.chord{67-59}-.letring{.chord{56-46}-----}", 1 },
        { ".letring{5?/54-44--45-46-}-|-.letring{5?/59-49--400-401};", 1 },
        { "63-45--63-45--55-43-;", 1 },
        { "62-54-42-44-;-62--54-42-44-32-42-4X-42h44-|-62--54-42-44-32-62-42h44-42h44-;", 1 },
        { "59-67-59-67-47-67-67-67-", 1 },
        { "67-67-55-67-62-62-65-66-", 1 },
        { ".mute{50-42-21-32-10-11-21-13-23-}-50-53~~-52-53-", 1 },
        { ".chord{50-42-32-21}-.chord{50-42-32-21}--.chord{45-34-23}--50-53~~-52-53-", 1 },
        { ".letring{67-59-49-37-49--49-59-49-37-49--49-47h49}-", 1 }
    };
    tulip_single_note_ctx *song = NULL;
    char errbuf[255] = "";
    size_t tlpcodes_nr = sizeof(tlpcodes) / sizeof(tlpcodes[0]);
    size_t t;
    int result = 0;
    for (t = 0; t < tlpcodes_nr; t++) {
        result = compile_tulip_codebuf(tlpcodes[t].codebuf, errbuf, &song, NULL);
        CUTE_ASSERT(result == tlpcodes[t].valid);
        if (tlpcodes[t].valid) {
            CUTE_ASSERT(song != NULL);
            free_tulip_single_note_ctx(song);
            song = NULL;
        } else {
            CUTE_ASSERT(song == NULL);
        }
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_utils_demux_tlp_commands_tests)
    tulip_command_t *demuxes = NULL;
    size_t demuxes_sz = 0;
    tulip_command_t techniques = kTlpMute            |
                                 kTlpLetRing         |
                                 kTlpChord           |
                                 kTlpStrum           |
                                 kTlpTremoloPicking  |
                                 kTlpVibrato         |
                                 kTlpSlideDown       |
                                 kTlpSlideUp         |
                                 kTlpHammerOn        |
                                 kTlpPullOff         |
                                 kTlpTimes           |
                                 kTlpVibratoWBar     |
                                 kTlpTunning         |
                                 kTlpLiteral         |
                                 kTlpSingleNote      |
                                 kTlpNoteSep         |
                                 kTlpSepBar          |
                                 kTlpSavePoint       |
                                 kTlpBend            |
                                 kTlpReleaseBend     |
                                 kTlpBlockEnd        |
                                 kTlpTapping         |
                                 kTlpNaturalHarmonic |
                                 kTlpArtificialHarmonic;
    tulip_command_t expected_demuxes[] = {
        kTlpSavePoint,
        kTlpNoteSep,
        kTlpBlockEnd,
        kTlpMute,
        kTlpLetRing,
        kTlpChord,
        kTlpStrum,
        kTlpTremoloPicking,
        kTlpVibratoWBar,
        kTlpTimes,
        kTlpVibrato,
        kTlpSlideDown,
        kTlpSlideUp,
        kTlpHammerOn,
        kTlpPullOff,
        kTlpTunning,
        kTlpLiteral,
        kTlpSingleNote,
        kTlpSepBar,
        kTlpBend,
        kTlpReleaseBend,
        kTlpTapping,
        kTlpNaturalHarmonic,
        kTlpArtificialHarmonic
    };
    demuxes = demux_tlp_commands(techniques, &demuxes_sz);
    CUTE_ASSERT(demuxes != NULL);
    CUTE_ASSERT(demuxes_sz == 24);
    while (demuxes_sz-- > 0) {
        CUTE_ASSERT(demuxes[demuxes_sz] == expected_demuxes[demuxes_sz]);
    }
    free(demuxes);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(processor_oututils_get_technique_label_tests)
    struct expected_labels_ctx {
        const char *label;
        tulip_command_t technique;
    };
    struct expected_labels_ctx expected_labels[] = {
        { "pm", kTlpMute               },
        { "lr", kTlpLetRing            },
        { "st", kTlpStrum              },
        { "tp", kTlpTremoloPicking     },
        { "~" , kTlpVibrato            },
        { "/" , kTlpSlideDown          },
        { "\\", kTlpSlideUp            },
        { "h" , kTlpHammerOn           },
        { "p" , kTlpPullOff            },
        { "vb", kTlpVibratoWBar        },
        { "-" , kTlpNoteSep            },
        { "|" , kTlpSepBar             },
        { "b" , kTlpBend               },
        { "r" , kTlpReleaseBend        },
        { "T" , kTlpTapping            },
        { "*" , kTlpNaturalHarmonic    },
        { "v" , kTlpArtificialHarmonic }
    };
    const size_t expected_labels_nr = sizeof(expected_labels) / sizeof(expected_labels[0]);
    size_t e = 0;
    char *label = NULL;
    for (e = 0; e < expected_labels_nr; e++) {
        label = get_technique_label(expected_labels[e].technique);
        CUTE_ASSERT(label != NULL);
        CUTE_ASSERT(strcmp(label, expected_labels[e].label) == 0);
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(processor_oututils_single_note_to_tab_fret_nr_tests)
    struct test_vector_ctx {
        const char *note;
        const int fret_nr;
    };
    struct test_vector_ctx test_vector[] = {
        { "10" ,  0 }, { "11" ,  1 }, { "12" ,  2 }, { "13" ,  3 }, { "14" ,  4 }, { "15" ,  5 }, { "16" ,  6 }, { "17" ,  7 }, { "18" ,  8 }, { "19" ,  9 }, { "100", 10 }, { "101", 11 }, { "102", 12 }, { "103", 13 }, { "104", 14 }, { "105", 15 }, { "106", 16 }, { "107", 17 }, { "108", 18 }, { "109", 19 }, { "110", 20 }, { "111", 21 }, { "112", 22 },
        { "20" ,  0 }, { "21" ,  1 }, { "22" ,  2 }, { "23" ,  3 }, { "24" ,  4 }, { "25" ,  5 }, { "26" ,  6 }, { "27" ,  7 }, { "28" ,  8 }, { "29" ,  9 }, { "200", 10 }, { "201", 11 }, { "202", 12 }, { "203", 13 }, { "204", 14 }, { "205", 15 }, { "206", 16 }, { "207", 17 }, { "208", 18 }, { "209", 19 }, { "210", 20 }, { "211", 21 }, { "212", 22 },
        { "30" ,  0 }, { "31" ,  1 }, { "32" ,  2 }, { "33" ,  3 }, { "34" ,  4 }, { "35" ,  5 }, { "36" ,  6 }, { "37" ,  7 }, { "38" ,  8 }, { "39" ,  9 }, { "300", 10 }, { "301", 11 }, { "302", 12 }, { "303", 13 }, { "304", 14 }, { "305", 15 }, { "306", 16 }, { "307", 17 }, { "308", 18 }, { "309", 19 }, { "310", 20 }, { "311", 21 }, { "312", 22 },
        { "40" ,  0 }, { "41" ,  1 }, { "42" ,  2 }, { "43" ,  3 }, { "44" ,  4 }, { "45" ,  5 }, { "46" ,  6 }, { "47" ,  7 }, { "48" ,  8 }, { "49" ,  9 }, { "400", 10 }, { "401", 11 }, { "402", 12 }, { "403", 13 }, { "404", 14 }, { "405", 15 }, { "406", 16 }, { "407", 17 }, { "408", 18 }, { "409", 19 }, { "410", 20 }, { "411", 21 }, { "412", 22 },
        { "50" ,  0 }, { "51" ,  1 }, { "52" ,  2 }, { "53" ,  3 }, { "54" ,  4 }, { "55" ,  5 }, { "56" ,  6 }, { "57" ,  7 }, { "58" ,  8 }, { "59" ,  9 }, { "500", 10 }, { "501", 11 }, { "502", 12 }, { "503", 13 }, { "504", 14 }, { "505", 15 }, { "506", 16 }, { "507", 17 }, { "508", 18 }, { "509", 19 }, { "510", 20 }, { "511", 21 }, { "512", 22 },
        { "60" ,  0 }, { "61" ,  1 }, { "62" ,  2 }, { "63" ,  3 }, { "64" ,  4 }, { "65" ,  5 }, { "66" ,  6 }, { "67" ,  7 }, { "68" ,  8 }, { "69" ,  9 }, { "600", 10 }, { "601", 11 }, { "602", 12 }, { "603", 13 }, { "604", 14 }, { "605", 15 }, { "606", 16 }, { "607", 17 }, { "608", 18 }, { "609", 19 }, { "610", 20 }, { "611", 21 }, { "612", 22 },
    };
    const size_t test_vector_nr = sizeof(test_vector) / sizeof(test_vector[0]);
    size_t t = 0;
    for (t = 0; t < test_vector_nr; t++) {
        CUTE_ASSERT(single_note_to_tab_fret_nr(test_vector[t].note) == test_vector[t].fret_nr);
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(system_get_tulip_system_version_tests)
    CUTE_ASSERT(get_tulip_system_version() != NULL);
CUTE_TEST_CASE_END

void write_buffer_to_disk(const char *filepath, const char * buffer, const size_t buffer_size) {
    FILE *fp = fopen(filepath, "wb");
    if (fp == NULL) {
        return;
    }
    fwrite(buffer, 1, buffer_size, fp);
    fclose(fp);
}

CUTE_TEST_CASE(system_tulip_task_exec_tests)
    //  WARN(Santiago): This test will test indirectly the tulip_system_init() function.
    //                  If it is failing nothing here will make sense too.
    struct task_ctx {
        int exit_code;
        int argc;
        char **argv;
    };
    struct task_ctx tasks[] = {
        { 1, 2, (char * []){"", "--duh"        } },
        { 0, 2, (char * []){"", "--help"       } },
        { 0, 2, (char * []){"", "--version"    } },
        { 1, 2, (char * []){"", "--tlp=unk.tlp"} },
        { 0, 2, (char * []){"", "--tlp=ok.tlp" } }
    };
    const size_t tasks_nr = sizeof(tasks) / sizeof(tasks[0]);
    const char *tlp_code = ".chord{60-52-42-31-20-10}-60-.letring{.chord{10-20}--------}";
    size_t t = 0;
    write_buffer_to_disk("ok.tlp", tlp_code, strlen(tlp_code));
    for (t = 0; t < tasks_nr; t++) {
        tulip_system_init(tasks[t].argc, tasks[t].argv);
        CUTE_ASSERT(tulip_task_exec() == tasks[t].exit_code);
    }
    remove("ok.tlp");
CUTE_TEST_CASE_END

CUTE_TEST_CASE(processor_typesetters_txt_tablature_ctx_tests)
    txttypesetter_tablature_ctx *tab = NULL, *head = NULL, *next = NULL;
    head = new_txttypesetter_tablature_ctx(&tab);
    CUTE_ASSERT(tab != NULL && tab == head);
    next = new_txttypesetter_tablature_ctx(&tab);
    CUTE_ASSERT(tab == head && tab->next == next && next->next == NULL);
    free_txttypesetter_tablature_ctx(tab);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(base_tulip_part_ctx_tests)
    tulip_part_ctx *parts = NULL;
    tulip_single_note_ctx *begin = (tulip_single_note_ctx *)0xdeadbeef;
    tulip_single_note_ctx *end   = (tulip_single_note_ctx *)0xbeefdead;
    parts = add_part_to_tulip_part_ctx(parts, NULL, begin, end);
    CUTE_ASSERT(parts == NULL);
    parts = add_part_to_tulip_part_ctx(parts, "(null)", NULL, end);
    CUTE_ASSERT(parts == NULL);
    parts = add_part_to_tulip_part_ctx(parts, "(null)", begin, NULL);
    CUTE_ASSERT(parts == NULL);
    parts = add_part_to_tulip_part_ctx(parts, "intro", begin, end);
    parts = add_part_to_tulip_part_ctx(parts, "outro", end, begin);
    CUTE_ASSERT(parts != NULL);
    CUTE_ASSERT(parts->label != NULL && parts->begin != NULL && parts->end != NULL);
    CUTE_ASSERT(strcmp(parts->label, "intro") == 0 && parts->begin == begin && parts->end == end);
    CUTE_ASSERT(parts->next != NULL);
    CUTE_ASSERT(parts->next->label != NULL && parts->next->begin != NULL && parts->next->end != NULL);
    CUTE_ASSERT(strcmp(parts->next->label, "outro") == 0 && parts->next->begin == end && parts->next->end == begin);
    free_tulip_part_ctx(parts);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_utils_get_cmd_tag_from_cmd_code_tests)
    struct testvector {
        const char *tag;
        const tulip_command_t code;
    };
    struct testvector expected[] = {
        {               ";", kTlpSavePoint          },
        {               "-", kTlpNoteSep            },
        {               "}", kTlpBlockEnd           },
        {           ".mute", kTlpMute               },
        {        ".letring", kTlpLetRing            },
        {          ".chord", kTlpChord              },
        {          ".strum", kTlpStrum              },
        { ".tremolopicking", kTlpTremoloPicking     },
        {    ".vibratowbar", kTlpVibratoWBar        },
        {          ".times", kTlpTimes              },
        {               "~", kTlpVibrato            },
        {               "/", kTlpSlideDown          },
        {              "\\", kTlpSlideUp            },
        {               "h", kTlpHammerOn           },
        {               "p", kTlpPullOff            },
        {        ".tunning", kTlpTunning            },
        {        ".literal", kTlpLiteral            },
        {               "|", kTlpSepBar             },
        {               "b", kTlpBend               },
        {               "r", kTlpReleaseBend        },
        {               "T", kTlpTapping            },
        {               "*", kTlpNaturalHarmonic    },
        {               "v", kTlpArtificialHarmonic },
        {               "@", kTlpOnceMore           },
        {           ".part", kTlpPart               },
        {         ".repeat", kTlpRepeat             },
        {           ".song", kTlpSong               },
        {    ".transcriber", kTlpTranscriber        }
    };
    const size_t expected_nr = sizeof(expected) / sizeof(expected[0]);
    size_t e;
    const char *tag = NULL;
    for (e = 0; e < expected_nr; e++) {
        tag = get_cmd_tag_from_cmd_code(expected[e].code);
        CUTE_ASSERT(tag != NULL);
        CUTE_ASSERT(strcmp(tag, expected[e].tag) == 0);
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_utils_has_sustained_technique_tests)
    struct test_vector {
        tulip_command_t technique;
        int expected;
    };
    struct test_vector tests[] = {
        {                              kTlpNone,  0 },
        {                         kTlpSavePoint,  0 },
        {                           kTlpNoteSep,  0 },
        {                          kTlpBlockEnd,  0 },
        {                              kTlpMute,  1 },
        {                           kTlpLetRing,  1 },
        {                             kTlpChord,  1 },
        {                             kTlpStrum,  1 },
        {                    kTlpTremoloPicking,  1 },
        {                       kTlpVibratoWBar,  1 },
        {                             kTlpTrill,  1 },
        {                             kTlpTimes,  0 },
        {                           kTlpVibrato,  0 },
        {                         kTlpSlideDown,  0 },
        {                           kTlpSlideUp,  0 },
        {                          kTlpHammerOn,  0 },
        {                           kTlpPullOff,  0 },
        {                           kTlpTunning,  0 },
        {                           kTlpLiteral,  0 },
        {                        kTlpSingleNote,  0 },
        {                            kTlpSepBar,  0 },
        {                              kTlpBend,  0 },
        {                       kTlpReleaseBend,  0 },
        {                           kTlpTapping,  0 },
        {                   kTlpNaturalHarmonic,  0 },
        {                kTlpArtificialHarmonic,  0 },
        {                              kTlpSong,  0 },
        {                       kTlpTranscriber,  0 },
        {                          kTlpOnceMore,  0 },
        {                              kTlpPart,  0 },
        {                            kTlpRepeat,  0 },
        { kTlpHammerOn | kTlpMute | kTlpVibrato,  1 },
        { kTlpPullOff | kTlpTrill | kTlpHammerOn, 1 },
        { kTlpVibratoWBar | kTlpMute | kTlpChord, 1 }
    };
    size_t tests_nr = sizeof(tests) / sizeof(tests[0]);
    size_t t = 0;
    for (t = 0; t < tests_nr; t++) {
        CUTE_ASSERT(has_sustained_technique(tests[t].technique) == tests[t].expected);
    }

CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_utils_has_non_sustained_technique_tests)
    struct test_vector {
        tulip_command_t technique;
        int expected;
    };
    struct test_vector tests[] = {
        {                              kTlpNone,  0 },
        {                         kTlpSavePoint,  0 },
        {                           kTlpNoteSep,  1 },
        {                          kTlpBlockEnd,  0 },
        {                              kTlpMute,  0 },
        {                           kTlpLetRing,  0 },
        {                             kTlpChord,  0 },
        {                             kTlpStrum,  0 },
        {                    kTlpTremoloPicking,  0 },
        {                       kTlpVibratoWBar,  0 },
        {                             kTlpTrill,  0 },
        {                             kTlpTimes,  0 },
        {                           kTlpVibrato,  1 },
        {                         kTlpSlideDown,  1 },
        {                           kTlpSlideUp,  1 },
        {                          kTlpHammerOn,  1 },
        {                           kTlpPullOff,  1 },
        {                           kTlpTunning,  0 },
        {                           kTlpLiteral,  0 },
        {                        kTlpSingleNote,  0 },
        {                            kTlpSepBar,  0 },
        {                              kTlpBend,  1 },
        {                       kTlpReleaseBend,  1 },
        {                           kTlpTapping,  1 },
        {                   kTlpNaturalHarmonic,  1 },
        {                kTlpArtificialHarmonic,  1 },
        {                              kTlpSong,  0 },
        {                       kTlpTranscriber,  0 },
        {                          kTlpOnceMore,  0 },
        {                              kTlpPart,  0 },
        {                            kTlpRepeat,  0 },
        { kTlpHammerOn | kTlpMute | kTlpVibrato,  1 },
        { kTlpPullOff | kTlpTrill | kTlpHammerOn, 1 },
        { kTlpVibratoWBar | kTlpMute | kTlpChord, 0 }
    };
    size_t tests_nr = sizeof(tests) / sizeof(tests[0]);
    size_t t = 0;
    for (t = 0; t < tests_nr; t++) {
        CUTE_ASSERT(has_non_sustained_technique(tests[t].technique) == tests[t].expected);
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(usrland_cmdlineoptions_tests)
    char *argv[] = {
        "--qotsa=qotsa,rater-r,songs-for-the-deaf,lullabies-to-paralyze,era-vulgaris,...like-clockwork",
        "--jimi-hendrix=are-you-experienced?,axis-bold-as-love,eletric-ladyland",
        "--black-sabbath=black-sabbath,paranoid,master-of-reality,vol.4,sabbath-bloody-sabbath,sabotage,technical-ecstasy,never-say-die!",
        "--awesome"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    struct test_vector {
        char *option;
        char *expected;
    };
    struct test_vector tests[] = {
        { "qotsa", "qotsa,rater-r,songs-for-the-deaf,lullabies-to-paralyze,era-vulgaris,...like-clockwork" },
        { "jimi-hendrix", "are-you-experienced?,axis-bold-as-love,eletric-ladyland" },
        { "black-sabbath", "black-sabbath,paranoid,master-of-reality,vol.4,sabbath-bloody-sabbath,sabotage,technical-ecstasy,never-say-die!" }
    };
    size_t tests_nr = sizeof(tests) / sizeof(tests[0]);
    size_t t = 0;
    const char *data = NULL;

    for (t = 0; t < tests_nr; t++) {
        CUTE_ASSERT(get_option(tests[t].option, NULL) == NULL);
    }

    set_option_argc_argv(argc, argv);

    for (t = 0; t < tests_nr; t++) {
        data = get_option(tests[t].option, NULL);
        CUTE_ASSERT(data != NULL);
        CUTE_ASSERT(strcmp(tests[t].expected, data) == 0);
    }

    CUTE_ASSERT(get_bool_option("these-droids-aren't-what-we-are-looking-for", 1) == 1);

    CUTE_ASSERT(get_bool_option("--awesome", 0) == 0);

    CUTE_ASSERT(get_bool_option("awesome", 0) == 1);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(dsl_compiler_fuzz_tests)
    size_t fuzz_nr = 10000;
    char *buf = NULL;
    size_t buf_sz = 0;
    size_t b = 0;
    tulip_single_note_ctx *song = NULL;

    srand(stime(0));

    printf("\tHello Human, I will try to compile %d random buffers of varied sizes in order to see if something can explode here...\n"
           "\tPlease wait...\n", fuzz_nr);
    while (fuzz_nr-- > 0) {
        buf_sz = rand() % 65535;
        buf = (char *) getseg(buf_sz + 1);
        memset(buf, 0, buf_sz + 1);
        for (b = 0; b < buf_sz; b++) {
            buf[b] = rand() % 255;
        }
        write_buffer_to_disk("fuzz.tlp", buf, buf_sz);  //  WARN(Santiago): If it can cause any unexpected behavior we need to known more about this buffer.
        compile_tulip_codebuf(buf, NULL, &song, NULL);
        if (song != NULL) {
            free_tulip_single_note_ctx(song);
            song = NULL;
        }
        free(buf);
        remove("fuzz.tlp");
    }
    printf("\tNice! We are alive! Nothing is exploding at our faces.\n");
CUTE_TEST_CASE_END

CUTE_TEST_CASE(processor_fancy_outputs_assurance)
    size_t t;
    tulip_single_note_ctx *song = NULL;
    char *output_buf = NULL;
    size_t output_buf_sz = 0, b = 0;
    FILE *output = NULL;
    for (t = 0; t < g_fancy_outputs_test_vector_nr; t++) {
        CUTE_ASSERT(compile_tulip_codebuf(g_fancy_outputs_test_vector[t].tlp_code, NULL, &song, NULL) == 1);
        CUTE_ASSERT(song != NULL);
        CUTE_ASSERT(mktab(song, "output.txt") == 0);
        free_tulip_single_note_ctx(song);
        song = NULL;
        output = fopen("output.txt", "rb");
        CUTE_ASSERT(output != NULL);
        fseek(output, 0L, SEEK_END);
        output_buf_sz = (size_t) ftell(output);
        fseek(output, 0L, SEEK_SET);
        CUTE_ASSERT(output_buf_sz == g_fancy_outputs_test_vector[t].txt_output_sz);
        output_buf = (char *) getseg(output_buf_sz + 1);
        memset(output_buf, 0, output_buf_sz + 1);
        fread(output_buf, 1, output_buf_sz, output);
        fclose(output);
        for (b = 0; b < output_buf_sz; b++) {
            CUTE_ASSERT(output_buf[b] == g_fancy_outputs_test_vector[t].txt_output[b]);
        }
        free(output_buf);
        remove("output.txt");
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(tulips_tester_monkey)
    CUTE_RUN_TEST(base_tulip_technique_stack_ctx_tests);
    CUTE_RUN_TEST(base_tulip_single_note_ctx_tests);
    CUTE_RUN_TEST(base_tulip_part_ctx_tests);
    CUTE_RUN_TEST(dsl_basic_dsl_utils_tests);
    CUTE_RUN_TEST(dsl_strutils_tests);
    CUTE_RUN_TEST(dsl_parser_skip_string_chunk_tests);
    CUTE_RUN_TEST(dsl_parser_get_next_tlp_technique_block_begin_tests);
    CUTE_RUN_TEST(dsl_parser_get_next_tlp_technique_block_end_tests);
    CUTE_RUN_TEST(dsl_parser_get_next_tlp_technique_block_size_tests);
    CUTE_RUN_TEST(dsl_parser_get_codebuf_from_filepath_tests);
    CUTE_RUN_TEST(dsl_parser_get_next_tlp_command_tests);
    CUTE_RUN_TEST(dsl_parser_set_curr_code_line_number_tests);
    CUTE_RUN_TEST(dsl_parser_get_curr_code_line_number_tests);
    CUTE_RUN_TEST(dsl_utils_tlp_cmd_code_to_plain_index_tests);
    CUTE_RUN_TEST(dsl_utils_demux_tlp_commands_tests);
    CUTE_RUN_TEST(dsl_utils_get_cmd_tag_from_cmd_code_tests);
    CUTE_RUN_TEST(dsl_utils_has_sustained_technique_tests);
    CUTE_RUN_TEST(dsl_utils_has_non_sustained_technique_tests);
    CUTE_RUN_TEST(dsl_compiler_compile_tulip_codebuf);
    if (CUTE_GET_OPTION("skip-fuzz-tests") == NULL) {
        CUTE_RUN_TEST(dsl_compiler_fuzz_tests);
    } else {
        printf("***\n*** WARNING: The compiler's fuzz tests were skipped.\n***\n");
    }
    //  WARN(Santiago): It is important to run the following test after
    //                  the test "dsl_utils_tlp_cmd_code_to_plain_index_tests"
    //                  because the following tested function is quite dependant
    //                  from the previous one. Being totally non-sense try to test
    //                  it with the another broken.
    CUTE_RUN_TEST(processor_oututils_get_technique_label_tests);
    CUTE_RUN_TEST(processor_oututils_single_note_to_tab_fret_nr_tests);
    CUTE_RUN_TEST(processor_typesetters_txt_tablature_ctx_tests);
    CUTE_RUN_TEST(usrland_cmdlineoptions_tests);
    //  WARN(Santiago): The tests related with the system module should
    //                  run after.
    CUTE_RUN_TEST(system_get_tulip_system_version_tests);
    CUTE_RUN_TEST(system_tulip_task_exec_tests);
    if (CUTE_GET_OPTION("skip-fancy-outputs-assurance") == NULL) {
        CUTE_RUN_TEST(processor_fancy_outputs_assurance);
    } else {
        printf("***\n*** WARNING: The fancy outputs assurance tests were skipped.\n***\n");
    }
CUTE_TEST_CASE_END

CUTE_MAIN(tulips_tester_monkey);
