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

    return false;
}

static bool lex_expect(lex *self, u8 c) {
    if (lex_go(self) && lex_at(self) == c) {
        lex_next(self);
        return true;
    }

    return false;
}

#define LEX_SKIP_UNTIL(p)                                                                          \
    while (true) {                                                                                 \
        if (!lex_go(self)) {                                                                       \
            return lex_ptr(self);                                                                  \
        }                                                                                          \
        u8 *ptr = lex_ptr(self);                                                                   \
        if (p) {                                                                                   \
            lex_next(self);                                                                        \
            return ptr;                                                                            \
        }                                                                                          \
        lex_next(self);                                                                            \
    }

static u8 *lex_skip_until_pred(lex *self, bool (*pred)(char)) { LEX_SKIP_UNTIL(pred(*ptr)); }

static u8 *lex_skip_until(lex *self, u8 c) { LEX_SKIP_UNTIL(*ptr == c); }

static u8 *lex_skip_while_pred(lex *self, bool (*pred)(char)) { LEX_SKIP_UNTIL(!pred(*ptr)); }

static u8 *lex_skip_while(lex *self, u8 c) { LEX_SKIP_UNTIL(*ptr != c); }

static tok lex_scan_doc(lex *self) {
    if (!lex_expect(self, '#')) {
        return tok_from_tag(TOK_ERR);
    }

    if (!lex_skip(self)) {
        return tok_new(TOK_DOC, slice_empty());
    }

    u8 *start = lex_ptr(self);
    u8 *end = lex_skip_until(self, '\n');
    return tok_new(TOK_DOC, SLICE(start, end));
}

static tok lex_scan_comment(lex *self) {
    if (!lex_skip(self)) {
        return tok_new(TOK_COMMENT, slice_empty());
    }

    u8 *start = lex_ptr(self);
    u8 *end = lex_skip_until(self, '\n');
    return tok_new(TOK_COMMENT, SLICE(start, end));
}

static tok lex_scan_attr(lex *self) {
    if (!lex_skip(self)) {
        return tok_new(TOK_ATTR, slice_empty());
    }

    u8 *start = lex_ptr(self);
    u8 *end = lex_skip_until(self, '\n');
    return tok_new(TOK_ATTR, SLICE(start, end));
}

static tok lex_scan_dec(lex *self) {
    u8 *start = lex_ptr(self);
    u8 *end = lex_skip_while_pred(self, is_dec);
    return tok_new(TOK_DEC, SLICE(start, end));
}

static tok lex_scan(lex *self) {
    if (!lex_skip(self)) {
        return tok_from_tag(TOK_END);
    }

    u8 c = lex_at(self);
    switch (c) {
    case '\n':
        lex_next(self);
        return tok_from_tag(TOK_NL);

    case '(':
        lex_next(self);
        return tok_from_tag(TOK_LPAR);

    case ')':
        lex_next(self);
        return tok_from_tag(TOK_RPAR);

    case '{':
        lex_next(self);
        return tok_from_tag(TOK_LBRC);

    case '}':
        lex_next(self);
        return tok_from_tag(TOK_RBRC);

    case '[':
        lex_next(self);
        return tok_from_tag(TOK_LSBR);

    case ']':
        lex_next(self);
        return tok_from_tag(TOK_RSBR);

    case ',':
        lex_next(self);
        return tok_from_tag(TOK_COMMA);

    case '#': {
        lex_next(self);
        _lex_state state = self->state;
        tok t = lex_scan_doc(self);
        if (t.tag == TOK_ERR) {
            self->state = state;
            t = lex_scan_comment(self);
        }

        return t;
    }

    case '@':
        lex_next(self);
        return lex_scan_attr(self);

    default:
        if (is_dec(c)) {
            return lex_scan_dec(self);
        }

        return tok_from_tag(TOK_ERR);
    }
}

#undef LEX_SKIP_UNTIL
