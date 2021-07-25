#pragma once

#include "../def.h"
#include "../lib/vec.h"
#include "tok.h"

typedef enum {
    PAR_RULE_TERM,
    PAR_RULE_EXPR,
} par_rule;

typedef struct {
    par_rule rule;
    tok_tag tag;
} par_sym;

static par_sym par_sym_new(par_rule rule) {
    return (par_sym){
        .rule = rule,
    };
}

static par_sym par_sym_from_tag(tok_tag tag) {
    return (par_sym){
        .rule = PAR_RULE_TERM,
        .tag = tag,
    };
}

typedef struct {
    vec stack;
} par;

static par par_new() {
    vec stack = vec_new();
    par_sym start = par_sym_new(PAR_RULE_EXPR);
    vec_push(&stack, &start, sizeof(par_sym));
    return (par){
        .stack = stack,
    };
}

static void par_drop(const par *self) { vec_drop(&self->stack); }

static void par_parse(par *self, slice input) {
    if (slice_len_bytes(input) == 0) {
        if (vec_len(self->stack) == 0) {
            return;
        }

        par_sym top = *(par_sym *)vec_top(self->stack);
        printf("expected rule: %d\n", top.rule);
        FAIL("empty stack");
    }

    tok inp = *(tok *)slice_start(input);
    par_sym top = *(par_sym *)vec_top(self->stack);
    if (top.rule == PAR_RULE_TERM) {
        if (top.tag == inp.tag) {
            vec_pop(self->stack, sizeof(par_sym));
            input = slice_tail(input, 1);
            return;
        } else {
            printf("expected term: %d\n", top.tag);
            FAIL("unexpected");
        }
    }

    switch (top.rule) {
    case PAR_RULE_EXPR: {
        switch (inp.tag) {
        case TOK_DEC: {
            printf("RULE: %d\n", PAR_RULE_EXPR);
            vec_pop(self->stack, sizeof(par_sym));
            par_sym sym = par_sym_from_tag(TOK_DEC);
            vec_push(self->stack, &sym, sizeof(par_sym));
            return;
        }
        default:
            break;
        }

        printf("expected rule: %d\n", top.rule);
        FAIL("empty stack");
    }
    default:
        UNREACHABLE;
    }
}
