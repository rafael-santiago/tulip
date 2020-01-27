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
    unsigned char *output;
    size_t output_sz;
    unsigned char *filepath;
};

#define add_new_output_fancy_testing_step(song_name, ext) { ___ ## song_name ## _tlp,\
              sizeof(___ ## song_name ## _tlp) / sizeof(___ ## song_name ## _tlp[0]),\
                                                        ___ ## song_name ## _ ## ext,\
     sizeof(___ ## song_name ## _ ## ext) / sizeof(___ ## song_name ## _ ## ext [0]),\
                                                                      "output." #ext }

struct typesetter_fancy_outputs_assurance g_fancy_outputs_test_vector[] = {
    add_new_output_fancy_testing_step(purple_haze, txt),
    add_new_output_fancy_testing_step(o_pato, txt),
    add_new_output_fancy_testing_step(feel_me, txt),
    add_new_output_fancy_testing_step(haitian_fight_song, txt),
    add_new_output_fancy_testing_step(high_heeled_sneakers, txt),
    add_new_output_fancy_testing_step(purple_haze, ps),
    add_new_output_fancy_testing_step(feel_me, ps),
    add_new_output_fancy_testing_step(born_to_hula, ps),
    add_new_output_fancy_testing_step(born_to_hula, md),
    add_new_output_fancy_testing_step(born_to_hula, html),
    add_new_output_fancy_testing_step(haitian_fight_song, eps),
    add_new_output_fancy_testing_step(pisces, svg)
};

size_t g_fancy_outputs_test_vector_nr = sizeof(g_fancy_outputs_test_vector) / sizeof(g_fancy_outputs_test_vector[0]);

#endif
