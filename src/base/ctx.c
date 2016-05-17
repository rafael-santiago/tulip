#include <base/ctx.h>
#include <base/memory.h>
#include <string.h>
#include <dsl/parser/parser.h>

#define new_technique_stack_ctx(t) ( (t) = (tulip_technique_stack_ctx *) getseg(sizeof(tulip_technique_stack_ctx)),\
                                     (t)->technique_code = kTlpNone, (t)->next = NULL )

#define new_tulip_single_note_ctx(t) ( (t) = (tulip_single_note_ctx *) getseg(sizeof(tulip_single_note_ctx)),\
                                       (t)->next = (t)->last = NULL, (t)->techniques = kTlpNone, (t)->line_nr = 0,\
                                       memset((t)->buf, 0, sizeof((t)->buf)) )

static tulip_single_note_ctx *get_tulip_single_note_ctx_tail(tulip_single_note_ctx *song);

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
    tulip_single_note_ctx *p;
    for (p = song; p->next != NULL; p = p->next);
    return p;
}

void free_tulip_single_note_ctx(tulip_single_note_ctx *song) {
    tulip_single_note_ctx *t, *p;
    for (t = p = song; t; p = t) {
        t = p->next;
        free(p);
    }
}
