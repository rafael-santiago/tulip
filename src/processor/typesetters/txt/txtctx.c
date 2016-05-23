/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <processor/typesetters/txt/txtctx.h>
#include <processor/oututils.h>
#include <base/memory.h>
#include <stdlib.h>
#include <string.h>

#define new_txttypesetter_comment_ctx(t) ( (t) = (txttypesetter_comment_ctx *) getseg(sizeof(txttypesetter_comment_ctx)),\
                                           (t)->data = NULL, (t)->next = NULL )

#define new_txttypesetter_sustained_technique_ctx(t) ( (t) = (txttypesetter_sustained_technique_ctx *) getseg(sizeof(txttypesetter_sustained_technique_ctx)),\
                                             (t)->data = NULL, (t)->next = NULL )

static txttypesetter_comment_ctx *get_txttypesetter_comment_ctx_tail(txttypesetter_comment_ctx *comments);

txttypesetter_comment_ctx *add_comment_to_txttypesetter_comment_ctx(txttypesetter_comment_ctx *comments, const char *comment, const size_t fretboard_sz) {
    txttypesetter_comment_ctx *head = comments, *p = NULL;
    if (comment == NULL) {
        return comments;
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
    strncpy(p->data, comment, fretboard_sz);
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
    }
}

txttypesetter_sustained_technique_ctx *push_technique_to_txttypesetter_sustained_technique_ctx(txttypesetter_sustained_technique_ctx *techniques, const tulip_command_t technique, const size_t fretboard_sz) {
    txttypesetter_sustained_technique_ctx *top = NULL;
    const char *technique_label = get_technique_label(technique);
    if (technique_label == NULL) {
        return techniques;
    }
    new_txttypesetter_sustained_technique_ctx(top);
    top->data = (char *) getseg(fretboard_sz + 1);
    memset(top->data, 0, fretboard_sz + 1);
    strncpy(top->data, technique_label, fretboard_sz);
    top->next = techniques;
    return top;
}

txttypesetter_sustained_technique_ctx *pop_technique_from_txttypesetter_sustained_technique_ctx(txttypesetter_sustained_technique_ctx *techniques) {
    txttypesetter_sustained_technique_ctx *top = NULL;
    if (techniques == NULL) {
        return NULL;
    }
    top = techniques->next;
    free(techniques->data);
    free(techniques);
    return top;
}

txttypesetter_sustained_technique_ctx *sustain_techniques(txttypesetter_sustained_technique_ctx *techniques, const size_t fretboard_sz) {
    txttypesetter_sustained_technique_ctx *tp = NULL;
    if (techniques == NULL) {
        return NULL;
    }
    for (tp = techniques; tp != NULL; tp = tp->next) {
        if (strnlen(tp->data, fretboard_sz) >= fretboard_sz) {
            continue;
        }
        strncat(tp->data, ".", fretboard_sz);
    }
    return techniques;
}

void free_txttypesetter_sustained_technique_ctx(txttypesetter_sustained_technique_ctx *techniques) {
    txttypesetter_sustained_technique_ctx *t, *p;
    for (t = p = techniques; t; p = t) {
        t = p->next;
        free(p->data);
    }
}
