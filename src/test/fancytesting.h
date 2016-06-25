/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_TESTS_FANCYTESTING_H
#define TULIP_TESTS_FANCYTESTING_H 1

#include "fancyvectors.h"

struct typesetter_fancy_outputs_assurance {
    unsigned char *tlp_code;
    size_t tlp_code_sz;
    unsigned char *txt_output;
    size_t txt_output_sz;
};

#define add_new_output_fancy_testing_step(tlp, txt) { tlp, sizeof(tlp) / sizeof(tlp[0]), txt, sizeof(txt) / sizeof(txt[0]) }

struct typesetter_fancy_outputs_assurance g_fancy_outputs_test_vector[] = {
    add_new_output_fancy_testing_step(___purple_haze_tlp, ___purple_haze_txt),
    add_new_output_fancy_testing_step(___o_pato_tlp, ___o_pato_txt),
    add_new_output_fancy_testing_step(___feel_me_tlp, ___feel_me_txt),
    add_new_output_fancy_testing_step(___haitian_fight_song_tlp, ___haitian_fight_song_txt),
    add_new_output_fancy_testing_step(___high_heeled_sneakers_tlp, ___high_heeled_sneakers_txt)
};

size_t g_fancy_outputs_test_vector_nr = sizeof(g_fancy_outputs_test_vector) / sizeof(g_fancy_outputs_test_vector[0]);

#endif
