/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/txtctx.h>
#include <processor/oututils.h>
#include <processor/settings.h>
#include <base/memory.h>
#include <stdlib.h>
#include <string.h>

#define new_txttypesetter_comment_ctx(t) ( (t) = (txttypesetter_comment_ctx *) getseg(sizeof(txttypesetter_comment_ctx)),\
                                           (t)->data = NULL, (t)->next = NULL )

#define new_txttypesetter_sustained_technique_ctx(t) ( (t) = (txttypesetter_sustained_technique_ctx *) getseg(sizeof(txttypesetter_sustained_technique_ctx)),\
                                             (t)->data = NULL, (t)->next = NULL, (t)->last = NULL )

#define new_tab_string(s, fts) (  (s) = (char *)getseg(fts + 1),\
                                  memset((s), 0, fts + 1),\
                                  memset((s), '-', fts) )

#define new_txttypesetter_active_technique_ctx(t) ( (t) = (txttypesetter_active_technique_ctx *) getseg(sizeof(txttypesetter_active_technique_ctx)),\
                                                    (t)->technique = kTlpNone,\
                                                    (t)->next = NULL,\
                                                    (t)->dp = NULL )

static txttypesetter_comment_ctx *get_txttypesetter_comment_ctx_tail(txttypesetter_comment_ctx *comments);

static txttypesetter_sustained_technique_ctx *get_txttypesetter_sustained_technique_ctx_tail(txttypesetter_sustained_technique_ctx *technique);

txttypesetter_comment_ctx *add_comment_to_txttypesetter_comment_ctx(txttypesetter_comment_ctx *comments, const char *comment, const size_t fretboard_sz) {
    txttypesetter_comment_ctx *head = comments, *p = NULL;
    const char *cp = NULL, *cp_end = NULL, *curr_comment = NULL;;

    if (comment == NULL) {
        return comments;
    }

    cp_end = comment + strlen(comment);
    cp = comment;

    while (cp != cp_end && strlen(cp) > fretboard_sz) {
        while (cp != cp_end && *cp != ' ') {
            cp++;
        }
        cp++;
    }

    if (head == NULL) {
        new_txttypesetter_comment_ctx(head);
        p = head;
    } else {
        p = get_txttypesetter_comment_ctx_tail(head);
        new_txttypesetter_comment_ctx(p->next);
        p = p->next;
    }

    p->data = (char *) getseg(fretboard_sz + 1);

    memset(p->data, 0, fretboard_sz + 1);

    memcpy(p->data, comment, cp_end - cp - 1 - (comment[cp_end - cp - 2] == ' ' ? 1 : 0));

    if ((cp_end - cp) < strlen(comment)) {
        if (comments == NULL) {
            comments = head;
        }
        head = add_comment_to_txttypesetter_comment_ctx(comments, &comment[cp_end - cp - 1], fretboard_sz);
    }

    return head;
}

static txttypesetter_comment_ctx *get_txttypesetter_comment_ctx_tail(txttypesetter_comment_ctx *comments) {
    txttypesetter_comment_ctx *p;
    if (comments == NULL) {
        return NULL;
    }
    for (p = comments; p->next != NULL; p = p->next);
    return p;
}

void free_txttypesetter_comment_ctx(txttypesetter_comment_ctx *comments) {
    txttypesetter_comment_ctx *t, *p;
    for (t = p = comments; t; p = t) {
        t = p->next;
        free(p->data);
        free(p);
    }
}

txttypesetter_sustained_technique_ctx *add_technique_to_txttypesetter_sustained_technique_ctx(txttypesetter_sustained_technique_ctx *techniques) {
    txttypesetter_sustained_technique_ctx *head = techniques, *tp = NULL;

    if (head == NULL) {
        new_txttypesetter_sustained_technique_ctx(head);
        tp = head;
    } else {
        tp = get_txttypesetter_sustained_technique_ctx_tail(techniques);
        new_txttypesetter_sustained_technique_ctx(tp->next);
        tp->next->last = tp;
        tp = tp->next;
    }

    size_t fretboard_sz = 0;
    fretboard_sz = *((size_t *) get_processor_setting("fretboard-size", NULL));
    tp->data = (char *) getseg(fretboard_sz + 1);
    memset(tp->data, 0, fretboard_sz + 1);
    memset(tp->data, ' ', fretboard_sz);

    return head;
}

static txttypesetter_sustained_technique_ctx *get_txttypesetter_sustained_technique_ctx_tail(txttypesetter_sustained_technique_ctx *technique) {
    txttypesetter_sustained_technique_ctx *tp;
    if (technique == NULL) {
        return NULL;
    }
    for (tp = technique; tp->next != NULL; tp = tp->next);
    return tp;
}

txttypesetter_sustained_technique_ctx *rm_technique_from_txttypesetter_sustained_technique_ctx(txttypesetter_sustained_technique_ctx *burn, txttypesetter_sustained_technique_ctx *techniques) {
    txttypesetter_sustained_technique_ctx *head = NULL;

    if (burn == NULL) {
        return techniques;
    }

    if (burn->last != NULL) {
        burn->last->next = burn->next;
    }

    if (burn->next != NULL) {
        burn->next->last = burn->last;
    }

    if (burn == techniques) {
        head = burn->next;
    } else {
        head = techniques;
    }

    burn->next = NULL;
    burn->last = NULL;

    free_txttypesetter_sustained_technique_ctx(burn);

    return head;
}

/*txttypesetter_sustained_technique_ctx *pop_technique_from_txttypesetter_sustained_technique_ctx(txttypesetter_sustained_technique_ctx *techniques) {
    txttypesetter_sustained_technique_ctx *top = NULL;
    if (techniques == NULL) {
        return NULL;
    }
    top = techniques->next;
    free(techniques->data);
    free(techniques);
    return top;
}*/

void sustain_technique(txttypesetter_sustained_technique_ctx **technique) {
    txttypesetter_sustained_technique_ctx *tp = NULL;
    size_t fretboard_sz = 0;
    if (technique == NULL) {
        return;
    }
    fretboard_sz = *((size_t *) get_processor_setting("fretboard-size", NULL));
    for (tp = *technique; tp != NULL; tp = tp->next) {
        strncat(tp->data, ".", fretboard_sz);
    }
}

void free_txttypesetter_sustained_technique_ctx(txttypesetter_sustained_technique_ctx *techniques) {
    txttypesetter_sustained_technique_ctx *t, *p;
    for (t = p = techniques; t; p = t) {
        t = p->next;
        free(p->data);
        free(p);
    }
}

txttypesetter_tablature_ctx *new_txttypesetter_tablature_ctx(txttypesetter_tablature_ctx **tablature) {
    txttypesetter_tablature_ctx *tail = NULL;
    char **tunning = NULL;

    if (tablature == NULL) {
        return NULL;
    }

    if ((*tablature) != NULL) {
        for (tail = (*tablature); tail->next != NULL; tail = tail->next);
        tail->next = (txttypesetter_tablature_ctx *) getseg(sizeof(txttypesetter_tablature_ctx));
        tail->next->last = tail;
        tail = tail->next;
    } else {
        (*tablature) = (txttypesetter_tablature_ctx *) getseg(sizeof(txttypesetter_tablature_ctx));
        tail = (*tablature);
        tail->last = NULL;
    }

    tail->fretboard_sz = *((size_t *) get_processor_setting("fretboard-size", NULL));

    tail->string_nr = 6;

    tail->song = NULL;
    tail->transcriber = NULL;

    tail->times = (char *) getseg(tail->fretboard_sz + 1);
    memset(tail->times, 0, tail->fretboard_sz + 1);
    memset(tail->times, ' ', tail->fretboard_sz);

    new_tab_string(tail->strings[0], tail->fretboard_sz);
    new_tab_string(tail->strings[1], tail->fretboard_sz);
    new_tab_string(tail->strings[2], tail->fretboard_sz);
    new_tab_string(tail->strings[3], tail->fretboard_sz);
    new_tab_string(tail->strings[4], tail->fretboard_sz);
    new_tab_string(tail->strings[5], tail->fretboard_sz);

    tunning = (char **) get_processor_setting("tunning", NULL);
    strncpy(tail->tunning[0], tunning[0], sizeof(tail->tunning[0]) - 1);
    strncpy(tail->tunning[1], tunning[1], sizeof(tail->tunning[1]) - 1);
    strncpy(tail->tunning[2], tunning[2], sizeof(tail->tunning[2]) - 1);
    strncpy(tail->tunning[3], tunning[3], sizeof(tail->tunning[3]) - 1);
    strncpy(tail->tunning[4], tunning[4], sizeof(tail->tunning[4]) - 1);
    strncpy(tail->tunning[5], tunning[5], sizeof(tail->tunning[5]) - 1);

    tail->next = NULL;

    tail->curr_row = 1;
    tail->curr_str = (tail->last != NULL) ? tail->last->curr_str : 1;
    tail->comments = NULL;
    tail->techniques = NULL;

    //  WARN(Santiago): Do not worry about sustained techniques present in the last fretboard diagram.
    //                  All will be reconstructed little by little in this next one. Specially the initial
    //                  technique indication label.

    tail->active_techniques = NULL;

    return tail;
}

void free_txttypesetter_tablature_ctx(txttypesetter_tablature_ctx *tablature) {
    txttypesetter_tablature_ctx *t = NULL, *p = NULL;
    size_t s;
    for (t = p = tablature; t; p = t) {
        t = p->next;
        free_txttypesetter_comment_ctx(p->comments);
        free_txttypesetter_sustained_technique_ctx(p->techniques);
        free_txttypesetter_active_technique_ctx(p->active_techniques);
        for (s = 0; s < 6; s++) {
            free(p->strings[s]);
        }
        free(p->times);
        if (p->song != NULL) {
            free(p->song);
        }
        if (p->transcriber != NULL) {
            free(p->transcriber);
        }
        free(p);
    }
}

txttypesetter_active_technique_ctx *push_technique_to_txttypesetter_active_technique_ctx(txttypesetter_active_technique_ctx *active_stack, const tulip_command_t technique, txttypesetter_sustained_technique_ctx **record_list, int *curr_row) {
    txttypesetter_active_technique_ctx *top = NULL, *ap = NULL;
    txttypesetter_sustained_technique_ctx *rp = NULL;
    int found = 0;
    char *label = NULL, *lp = NULL;

    if (record_list == NULL) {
        return active_stack;
    }

    new_txttypesetter_active_technique_ctx(top);
    top->next = active_stack;
    top->technique = technique;

    for (rp = *record_list; rp != NULL && top->dp == NULL; rp = rp->next) {

        found = 0;
        for (ap = active_stack; ap != NULL && !found; ap = ap->next) {
            found = (rp->data == ap->dp);
        }

        if (!found) {
            top->dp = rp->data;
        }
    }

    if (top->dp == NULL) {
        *record_list = add_technique_to_txttypesetter_sustained_technique_ctx(*record_list);
        rp = get_txttypesetter_sustained_technique_ctx_tail(*record_list);
        top->dp = rp->data;
    }

    label = get_technique_label(technique);

    if (label != NULL) {
        lp = label;
        //  WARN(Santiago): This avoid ugly gaps with ' '.
        sustain_active_techniques(top->next, strlen(lp), *curr_row);
        while (*lp != 0) {
            top->dp[(*curr_row)++] = *lp;
            lp++;
        }
    }

    return top;
}

void sustain_active_techniques(txttypesetter_active_technique_ctx *actives, const int dot_nr, const int curr_row) {
    txttypesetter_active_technique_ctx *ap = NULL;

    for (ap = actives; ap != NULL; ap = ap->next) {
        memset(&ap->dp[curr_row], '.', dot_nr);
    }
}

txttypesetter_active_technique_ctx *pop_technique_from_txttypesetter_active_technique_ctx(txttypesetter_active_technique_ctx *active_stack) {
    txttypesetter_active_technique_ctx *top = NULL;
    if (active_stack == NULL) {
        return NULL;
    }
    top = active_stack->next;
    free(active_stack);
    return top;
}

void free_txttypesetter_active_technique_ctx(txttypesetter_active_technique_ctx *active_stack) {
    txttypesetter_active_technique_ctx *t, *p;
    for (t = p = active_stack; t != NULL; p = t) {
        t = p->next;
        free(p);
    }
}
