#include <base/ctx.h>
#include <base/memory.h>

#define new_technique_stack_ctx(t) ( (t) = (tulip_technique_stack_ctx *) getseg(sizeof(tulip_technique_stack_ctx)),\
                                     (t)->technique_code = kTlpNone, (t)->next = NULL )

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
