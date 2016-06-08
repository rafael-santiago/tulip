/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <dsl/compiler/verifiers/oncemore.h>
#include <dsl/utils.h>
#include <base/ctx.h>

static tulip_single_note_ctx *find_oncemore_from_tlp_block(tulip_single_note_ctx *song);

static void hash_techniques(tulip_command_t *array, size_t array_size);

static int are_similar_hashes(const tulip_command_t *a, const size_t a_s, const tulip_command_t *b, const size_t b_s);

static int are_similar_hashes(const tulip_command_t *a, const size_t a_s, const tulip_command_t *b, const size_t b_s) {
    size_t a_c = 0, b_c = 0;
    int equals_nr = 0;
    if (a == NULL || b == NULL) {
        return 1;
    }
    for (a_c = 0; a_c < a_s; a_c++) {
        if (a[a_c] == kTlpNone) {
            continue;
        }
        for (b_c = 0; b_c < b_s; b_c++) {
            if (b[b_c] == kTlpNone) {
                continue;
            }
            equals_nr += (a[a_c] == b[b_c]);
        }
    }
    return (equals_nr == 0);
}

static void hash_techniques(tulip_command_t *array, size_t array_size) {
    size_t a = 0;

    if (array == NULL) {
        return;
    }

    for (a = 0; a < array_size; a++) {
        if (!has_sustained_technique(array[a])) {
            array[a] = kTlpNone;
        }
    }
}

/*void show_array(tulip_command_t *a, size_t s) {
    size_t i;
    for (i = 0; i < s; i++) {
        printf("%.8x\n", a[i]);
    }
    printf("--\n");
}*/

static tulip_single_note_ctx *find_oncemore_from_tlp_block(tulip_single_note_ctx *song) {
    tulip_command_t *te_a = NULL, *te_b = NULL;
    size_t te_a_sz = 0, te_b_sz = 0, t = 0;
    tulip_single_note_ctx *sp = NULL;

    if (song == NULL) {
        return NULL;
    }

    sp = song;

    te_a = demux_tlp_commands(sp->techniques, &te_a_sz);
    hash_techniques(te_a, te_a_sz);

    te_b = demux_tlp_commands(sp->techniques, &te_b_sz);
    hash_techniques(te_b, te_b_sz);

    while (sp != NULL && !are_similar_hashes(te_a, te_a_sz, te_b, te_b_sz)) {
        sp = sp->last;
        while (sp->techniques == kTlpBlockEnd) {
            sp = sp->last;
        }
        free(te_b);
        te_b = demux_tlp_commands(sp->techniques, &te_b_sz);
        hash_techniques(te_b, te_b_sz);
    }

    free(te_a);
    free(te_b);

    return sp;
}

tulip_single_note_ctx *find_oncemore_begin(tulip_single_note_ctx *song) {
    tulip_single_note_ctx *mp = NULL;

    if (song == NULL) {
        return NULL;
    }

    switch (song->techniques) {

        case kTlpBlockEnd:
            mp = song->last;
            while (mp->techniques & kTlpBlockEnd) {
                mp = mp->last;
            }
            mp = find_oncemore_from_tlp_block(mp);
            //  INFO(Santiago): Adding a note separator in order to get a more fancy typesetting from
            //                  this "once more".
            song = add_note_to_tulip_single_note_ctx(song, get_used_techniques() | kTlpNoteSep, NULL);
            break;

        default:
            mp = song;
            if (mp->techniques & kTlpSepBar) {
                mp = mp->last;
                while (mp != NULL && (mp->techniques & kTlpSepBar) == 0) {
                    mp = mp->last;
                }
                mp = mp->next;
            } else if ((mp->techniques & kTlpSingleNote) && mp->last != NULL) {
                if ((mp->last->techniques & kTlpHammerOn   ) ||
                    (mp->last->techniques & kTlpPullOff    ) ||
                    (mp->last->techniques & kTlpSlideDown  ) ||
                    (mp->last->techniques & kTlpSlideUp    )) {
                    mp = mp->last;
                    if (mp->last != NULL && (mp->techniques & kTlpSingleNote)) {
                        mp = find_oncemore_begin(mp->last);
                    }
                }
            } else if (mp->techniques & kTlpTimes && mp->last != NULL) {
                //  WARN(Santiago): This is a kind of naive construction, anyway let's try to
                //                  reduce the badness here.
                mp = find_oncemore_begin(mp->last);
            } else if (mp->techniques & kTlpNoteSep && mp->last != NULL) {
                //  WARN(Santiago): Yes, this is the same of the kTlpTimes rules. However,
                //                  these rules do not have any relation. In case of a further
                //                  change behavior in one of them this will not mess with the
                //                  another one.
                mp = find_oncemore_begin(mp->last);
            }
            break;

    }

    return mp;
}

int oncemore_verifier(const char *buf, char *error_message, tulip_single_note_ctx **song, const char **next) {

    tulip_single_note_ctx *mp = NULL, *mp_end = NULL, *sp = NULL;

    if (buf == NULL || song == NULL || next == NULL) {
        return 0;
    }

    if (get_cmd_code_from_cmd_tag(buf) != kTlpOnceMore) {
        tlperr_s(error_message, "The once more token was expected.");
        return 0;
    }

    sp = (*song);
    while (sp->next != NULL) {
        sp = sp->next;
    }

    if (sp->last == NULL) {
        tlperr_s(error_message, "There is nothing to be done once more here.");
        return 0;
    }

    mp_end = sp;
    mp = find_oncemore_begin(sp);
    if (mp == NULL) {
        mp = (*song);
    }

    //  INFO(Santiago): For our mirroring at least mp_end must be added to the song context.

    while (mp != mp_end) {
        (*song) = add_note_to_tulip_single_note_ctx((*song), mp->techniques, mp->buf);
        mp = mp->next;
    }
    (*song) = add_note_to_tulip_single_note_ctx((*song), mp_end->techniques , mp_end->buf);
    (*next) = buf + 1;

    return 1;
}
