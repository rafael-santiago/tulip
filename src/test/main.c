#include <cutest.h>
#include <base/types.h>
#include <base/ctx.h>
#include <dsl/utils.h>
#include <dsl/str/strutils.h>
#include <stdlib.h>

CUTE_TEST_CASE(tulip_technique_stack_ctx_tests)
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

CUTE_TEST_CASE(basic_dsl_utils_tests)
    //  INFO(Santiago): it tests the basic aspects of the DSL.
    struct expected_results {
        const char *tech;
        tulip_command_t cmd;
        int sustain;
    };
    struct expected_results e_results[] = {
        {           ".mute",           kTlpMute, 1},
        {        ".letring",        kTlpLetRing, 1},
        {          ".chord",          kTlpChord, 1},
        {           ".beat",           kTlpBeat, 1},
        { ".tremolopicking", kTlpTremoloPicking, 1},
        {    ".vibratowbar",    kTlpVibratoWBar, 1},
        {               "~",        kTlpVibrato, 0},
        {               "/",      kTlpSlideDown, 0},
        {              "\\",        kTlpSlideUp, 0},
        {               "h",       kTlpHammerOn, 0},
        {               "p",        kTlpPullOff, 0},
        {        ".tunning",        kTlpTunning, 0},
        {        ".literal",        kTlpLiteral, 0},
        {              "34",     kTlpSingleNote, 0},
        {               "-",        kTlpNoteSep, 0},
        {          "(null)",           kTlpNone, 0}
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
    string = expand_string("\"\t\n\r\"\"");
    CUTE_ASSERT(string != NULL && strlen(string) == 4 && string[0] == '\t' &&
                                                         string[1] == '\n' &&
                                                         string[2] == '\r' &&
                                                         string[3] == '"'  &&
                                                         string[4] == 0);
    free(string);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(tulip_tests)
    CUTE_RUN_TEST(tulip_technique_stack_ctx_tests);
    CUTE_RUN_TEST(basic_dsl_utils_tests);
    CUTE_RUN_TEST(dsl_strutils_tests);
CUTE_TEST_CASE_END

CUTE_MAIN(tulip_tests);
