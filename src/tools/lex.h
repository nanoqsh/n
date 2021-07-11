#pragma once

#include "../def.h"
#include "../lib/vec.h"
#include "tok.h"

static bool is_dec(char c) { return c >= '0' && c <= '9'; }

static bool is_hex(char c) { return is_dec(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }

typedef struct {
    u8 *ptr;
    word ln;
    word col;
    bool skip_line;
} _lex_state;

typedef struct {
    slice src;
    _lex_state state;
} lex;

static lex lex_new(slice src) {
    return (lex){
        .src = src,
        .state =
            {
                .ptr = src.start,
                .ln = 0,
                .col = 0,
                .skip_line = true,
            },
    };
}

static u8 *lex_ptr(lex *self) { return self->state.ptr; }

static u8 lex_at(lex *self) { return *lex_ptr(self); }

static word lex_ln(lex *self) { return self->state.ln; }

static word lex_col(lex *self) { return self->state.col; }

static bool lex_go(lex *self) { return lex_ptr(self) != self->src.end; }

static void lex_next(lex *self) {
    if (lex_at(self) == '\n') {
        _lex_state *state = &self->state;
        ++state->ln;
        state->col = 0;
        state->skip_line = true;
    } else {
        ++self->state.col;
    }

    ++self->state.ptr;
}

static slice lex_tail(lex *self) { return SLICE(self->state.ptr, self->src.end); }

static bool lex_skip(lex *self) {
    for (; lex_go(self); lex_next(self)) {
        switch (lex_at(self)) {
        case ' ':
        case '\t':
            break;

        case '\n':
            if (self->state.skip_line) {
                break;
            }

        default:
            self->state.skip_line = false;
            return true;
        }
    }

    return true;
}

static bool lex_expect_char(lex *self, u8 c) {
    if (!lex_go(self) || lex_at(self) != c) {
        return false;
    }

    lex_next(self);
    return true;
}

static bool lex_expect_char_pred(lex *self, hof pred) {
    if (!lex_go(self)) {
        return false;
    }

    u8 c = lex_at(self);
    if (!hof_call(pred, &c)) {
        return false;
    }

    lex_next(self);
    return true;
}

static bool lex_expect_str(lex *self, slice str) {
    slice tail = lex_tail(self);
    if (!slice_starts_with(tail, str)) {
        return false;
    }

    for (word i = 0; i < slice_len_bytes(str); ++i) {
        lex_next(self);
    }
    return true;
}

static bool lex_expect_while(lex *self, hof exp) {
    while (hof_call(exp, self))
        ;
    return true;
}

static bool lex_expect_n(lex *self, word n, hof exp) {
    _lex_state state = self->state;
    for (word i = 0; i < n; ++i) {
        if (!hof_call(exp, self)) {
            self->state = state;
            return false;
        }
    }

    return true;
}

static tok lex_scan(lex *self) {
    lex_skip(self);
    if (!lex_go(self)) {
        return tok_from_tag(TOK_END);
    }

    return tok_from_tag(TOK_ERR);
}
