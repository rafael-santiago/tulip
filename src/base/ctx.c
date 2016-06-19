/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <base/ctx.h>
#include <base/memory.h>
#include <string.h>
#include <dsl/parser/parser.h>

#define new_technique_stack_ctx(t) ( (t) = (tulip_technique_stack_ctx *) getseg(sizeof(tulip_technique_stack_ctx)),\
                                     (t)->technique_code = kTlpNone, (t)->next = NULL )

#define new_tulip_single_note_ctx(t) ( (t) = (tulip_single_note_ctx *) getseg(sizeof(tulip_single_note_ctx)),\
                                       (t)->next = (t)->last = NULL, (t)->techniques = kTlpNone, (t)->line_nr = 0,\
                                       memset((t)->buf, 0, sizeof((t)->buf)) )

#define new_tulip_part_ctx(t) ( (t) = (tulip_part_ctx *) getseg(sizeof(tulip_part_ctx)),\
                                (t)->next = NULL, (t)->label = NULL, (t)->begin = (t)->end = NULL )

static tulip_single_note_ctx *get_tulip_single_note_ctx_tail(tulip_single_note_ctx *song);

static tulip_part_ctx *get_tulip_part_ctx_tail(tulip_part_ctx *parts);

tulip_technique_stack_ctx *push_technique_to_technique_stack_ctx(tulip_technique_stack_ctx *stack, tulip_command_t technique) {
    tulip_technique_stack_ctx *top = NULL;
    new_technique_stack_ctx(top);
    top->technique_code = technique;
    top->next = stack;
    return top;
}

tulip_technique_stack_ctx *pop_technique_from_technique_stack_ctx(tulip_technique_stack_ctx *stack) {
    tulip_technique_stack_ctx *top = NULL;
    if (stack == NULL) {
        return NULL;
    }
    top = stack->next;
    free(stack);
    return top;
}

int is_empty_technique_stack_ctx(const tulip_technique_stack_ctx *stack) {
    return (stack == NULL);
}

tulip_command_t top_of_technique_stack_ctx(const tulip_technique_stack_ctx *stack) {
    if (stack == NULL) {
        return kTlpNone;
    }
    return stack->technique_code;
}

void free_technique_stack_ctx(tulip_technique_stack_ctx *stack) {
    tulip_technique_stack_ctx *t, *p;
    for (t = p = stack; t; p = t) {
        t = p->next;
        free(p);
    }
}

tulip_single_note_ctx *add_note_to_tulip_single_note_ctx(tulip_single_note_ctx *song, tulip_command_t techniques, const char *buf) {
    tulip_single_note_ctx *new = NULL, *last = NULL;
    tulip_single_note_ctx *head = song;
    if (head == NULL) {
        new_tulip_single_note_ctx(head);
        new = head;
    } else {
        last = get_tulip_single_note_ctx_tail(head);
        new_tulip_single_note_ctx(last->next);
        new = last->next;
        new->last = last;
    }
    new->techniques = techniques;
    if (buf != NULL) {
        strncpy(new->buf, buf, sizeof(new->buf) - 1);
    }
    new->line_nr = get_curr_code_line_number();
    return head;
}

static tulip_single_note_ctx *get_tulip_single_note_ctx_tail(tulip_single_note_ctx *song) {
    tulip_single_note_ctx *p = NULL;
    for (p = song; p->next != NULL; p = p->next);
    return p;
}

void tulip_single_note_ctx_cpy(tulip_single_note_ctx **song, const tulip_single_note_ctx *begin, const tulip_single_note_ctx *end) {
    const tulip_single_note_ctx *bp = NULL;
    if (song == NULL || begin == NULL || end == NULL) {
        return;
    }

    bp = begin;

    while (bp != end) {
        if (bp->techniques != kTlpSavePoint && bp->techniques != kTlpLiteral) {
            (*song) = add_note_to_tulip_single_note_ctx((*song), bp->techniques, bp->buf);
        }
        bp = bp->next;
    }

    if (end->techniques != kTlpSavePoint && bp->techniques != kTlpLiteral) {
        (*song) = add_note_to_tulip_single_note_ctx((*song), end->techniques , end->buf);
    }
}

void free_tulip_single_note_ctx(tulip_single_note_ctx *song) {
    tulip_single_note_ctx *t, *p;
    for (t = p = song; t; p = t) {
        t = p->next;
        free(p);
    }
}

static tulip_part_ctx *get_tulip_part_ctx_tail(tulip_part_ctx *parts) {
    tulip_part_ctx *p = NULL;
    if (parts == NULL) {
        return NULL;
    }
    for (p = parts; p->next != NULL; p = p->next);
    return p;
}

tulip_part_ctx *get_tulip_part_ctx(const char *label, tulip_part_ctx *parts) {
    tulip_part_ctx *p = NULL;
    for (p = parts; p != NULL; p = p->next) {
        if (strcmp(p->label, label) == 0) {
            return p;
        }
    }
    return NULL;
}

tulip_part_ctx *add_part_to_tulip_part_ctx(tulip_part_ctx *parts, const char *label, const tulip_single_note_ctx *begin, const tulip_single_note_ctx *end) {
    tulip_part_ctx *head = parts, *p = NULL;
    size_t s = 0;

    if (label == NULL || begin == NULL || end == NULL) {
        return NULL;
    }

    if (head == NULL) {
        new_tulip_part_ctx(head);
        p = head;
    } else {
        p = get_tulip_part_ctx_tail(head);
        new_tulip_part_ctx(p->next);
        p = p->next;
    }

    p->label = (char *) getseg((s = strlen(label)) + 1);
    memset(p->label, 0, s + 1);
    memcpy(p->label, label, s);
    p->begin = begin;
    p->end = end;

    return head;
}

void free_tulip_part_ctx(tulip_part_ctx *parts) {
    tulip_part_ctx *p, *t;
    for (t = p = parts; t; p = t) {
        t = p->next;
        free(p->label);
        free(p);
    }
}
