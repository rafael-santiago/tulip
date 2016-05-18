#include <cutest.h>
#include <base/types.h>
#include <base/ctx.h>
#include <dsl/utils.h>
#include <dsl/str/strutils.h>
#include <dsl/parser/parser.h>
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
        {           ".beat",               kTlpBeat, 1 },
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
    const char *ldbuf = NULL;
    FILE *fp = fopen("test.tlp", "wb");
    CUTE_ASSERT(fp != NULL);
    fwrite(codebuf, 1, strlen(codebuf), fp);
    fclose(fp);
    ldbuf = get_codebuf_from_filepath("test.tlp");
    remove("test.tlp");
    CUTE_ASSERT(ldbuf != NULL);
    CUTE_ASSERT(strcmp(ldbuf, codebuf) == 0);
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
        {               kTlpMute,  0 },
        {            kTlpLetRing,  1 },
        {              kTlpChord,  2 },
        {               kTlpBeat,  3 },
        {     kTlpTremoloPicking,  4 },
        {            kTlpVibrato,  5 },
        {          kTlpSlideDown,  6 },
        {            kTlpSlideUp,  7 },
        {           kTlpHammerOn,  8 },
        {            kTlpPullOff,  9 },
        {        kTlpVibratoWBar, 10 },
        {            kTlpTunning, 11 },
        {            kTlpLiteral, 12 },
        {         kTlpSingleNote, 13 },
        {            kTlpNoteSep, 14 },
        {             kTlpSepBar, 15 },
        {          kTlpSavePoint, 16 },
        {               kTlpBend, 17 },
        {        kTlpReleaseBend, 18 },
        {           kTlpBlockEnd, 19 },
        {            kTlpTapping, 20 },
        {    kTlpNaturalHarmonic, 21 },
        { kTlpArtificialHarmonic, 22 }
    };
    size_t values_nr = sizeof(values) / sizeof(values[0]), v;
    for (v = 0; v < values_nr; v++) {
        CUTE_ASSERT(tlp_cmd_code_to_plain_index(values[v].code) == values[v].index);
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(tulip_tests)
    CUTE_RUN_TEST(base_tulip_technique_stack_ctx_tests);
    CUTE_RUN_TEST(base_tulip_single_note_ctx_tests);
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
CUTE_TEST_CASE_END

CUTE_MAIN(tulip_tests);
