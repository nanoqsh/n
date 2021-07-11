#pragma once

#include "../def.h"
#include "../lib/vec.h"
#include "tok.h"

static bool is_bin(char c) { return c == '0' || c == '1' || c == '_'; }

static bool is_oct(char c) { return (c >= '0' && c <= '7') || c == '_'; }

static bool is_dec(char c) { return (c >= '0' && c <= '9') || c == '_'; }

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

static u8 *lex_next(lex *self) {
    if (lex_at(self) == '\n') {
        _lex_state *state = &self->state;
        ++state->ln;
        state->col = 0;
        state->skip_line = true;
    } else {
        ++self->state.col;
    }

    u8 *ptr = self->state.ptr;
    ++self->state.ptr;
    return ptr;
}

static slice lex_tail(lex *self) { return SLICE(self->state.ptr, self->src.end); }

static void lex_skip(lex *self) {
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
            return;
        }
    }

    return;
}

static u8 *lex_expect_char(lex *self, u8 c) {
    if (lex_go(self) && lex_at(self) == c) {
        return lex_next(self);
    }

    return NULL;
}

static u8 *lex_expect_pred(lex *self, hof pred) {
    if (hof_call(pred, self)) {
        return lex_ptr(self);
    }

    return NULL;
}

static u8 *lex_expect_str(lex *self, slice str) {
    slice tail = lex_tail(self);
    if (!slice_starts_with(tail, str)) {
        return NULL;
    }

    for (word i = 0; i < slice_len_bytes(str); ++i) {
        lex_next(self);
    }

    return lex_ptr(self);
}

static u8 *lex_expect_while(lex *self, hof exp) {
    while (hof_call(exp, self))
        ;

    return lex_ptr(self);
}

static bool _exp_not_at(lex *self, const char *c) {
    if (lex_go(self) && lex_at(self) != *c) {
        lex_next(self);
        return true;
    }

    return false;
}

static bool _exp_pred(lex *self, bool (*pred)(char)) {
    if (lex_go(self) && pred(lex_at(self))) {
        lex_next(self);
        return true;
    }

    return false;
}

static tok lex_scan(lex *self) {
    lex_skip(self);
    if (!lex_go(self)) {
        return tok_from_tag(TOK_END);
    }

    char nl = '\n';
    if (lex_expect_char(self, nl)) {
        return tok_from_tag(TOK_NL);
    }

    if (lex_expect_char(self, '(')) {
        return tok_from_tag(TOK_LPAR);
    }

    if (lex_expect_char(self, ')')) {
        return tok_from_tag(TOK_RPAR);
    }

    if (lex_expect_char(self, '{')) {
        return tok_from_tag(TOK_LBRC);
    }

    if (lex_expect_char(self, '}')) {
        return tok_from_tag(TOK_RBRC);
    }

    if (lex_expect_char(self, '[')) {
        return tok_from_tag(TOK_LSBR);
    }

    if (lex_expect_char(self, ']')) {
        return tok_from_tag(TOK_RSBR);
    }

    if (lex_expect_char(self, ',')) {
        return tok_from_tag(TOK_COMMA);
    }

    u8 *start;
    u8 *end;

    if (lex_expect_char(self, '#')) {
        tok_tag tag = lex_expect_char(self, '#') ? TOK_DOC : TOK_COMMENT;
        lex_skip(self);
        start = lex_ptr(self);
        end = lex_expect_while(self, HOF_WITH(_exp_not_at, &nl));
        ASSERT(lex_expect_char(self, nl));
        return tok_new(tag, SLICE(start, end));
    }

    if (lex_expect_char(self, '@')) {
        lex_skip(self);
        start = lex_ptr(self);
        end = lex_expect_while(self, HOF_WITH(_exp_not_at, &nl));
        ASSERT(lex_expect_char(self, nl));
        return tok_new(TOK_ATTR, SLICE(start, end));
    }

    hof p = HOF_WITH(_exp_pred, is_dec);
    start = lex_ptr(self);
    if (lex_expect_char(self, '0')) {
        tok_tag tag;
        if (lex_expect_char(self, 'b')) {
            p = HOF_WITH(_exp_pred, is_bin);
            tag = TOK_BIN;
        } else if (lex_expect_char(self, 'o')) {
            p = HOF_WITH(_exp_pred, is_oct);
            tag = TOK_OCT;
        } else if (lex_expect_char(self, 'x')) {
            p = HOF_WITH(_exp_pred, is_hex);
            tag = TOK_HEX;
        } else {
            end = lex_expect_while(self, p);
            return tok_new(TOK_DEC, SLICE(start, end));
        }

        if (!lex_expect_pred(self, p)) {
            return tok_from_tag(TOK_ERR);
        }

        end = lex_expect_while(self, p);
        return tok_new(tag, SLICE(start, end));
    }

    if (lex_expect_pred(self, p)) {
        end = lex_expect_while(self, p);
        return tok_new(TOK_DEC, SLICE(start, end));
    }

    const slice KW_FALSE = slice_from_str("false");
    const slice KW_TRUE = slice_from_str("true");
    if ((end = lex_expect_str(self, KW_FALSE)) || (end = lex_expect_str(self, KW_TRUE))) {
        return tok_new(TOK_BOOL, SLICE(start, end));
    }

    return tok_from_tag(TOK_ERR);
}
