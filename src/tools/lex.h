#pragma once

#include "../def.h"
#include "../lib/vec.h"
#include "tok.h"

typedef struct {
    slice src;
    u8 *ptr;
    tok_tag tok;
    word ln;
    word col;
    bool skip_line;
} lex;

static lex lex_new(slice src) {
    return (lex){
        .src = src,
        .ptr = src.start,
        .tok = 0,
        .ln = 0,
        .col = 0,
        .skip_line = true,
    };
}

static bool lex_go(lex *self) { return self->ptr != self->src.end; }

static void lex_next(lex *self) {
    if (*self->ptr == '\n') {
        ++self->ln;
        self->col = 0;
        self->skip_line = true;
    } else {
        ++self->col;
    }

    ++self->ptr;
}

static bool lex_skip(lex *self) {
    for (; lex_go(self); lex_next(self)) {
        switch (*self->ptr) {
        case ' ':
        case '\t':
            break;

        case '\n':
            if (self->skip_line) {
                break;
            }

        default:
            self->skip_line = false;
            return true;
        }
    }

    return false;
}

static bool lex_expect_char(lex *self, u8 c) { return lex_go(self) && *self->ptr == c; }

static u8 *lex_skip_until(lex *self, u8 c) {
    while (true) {
        if (!lex_go(self)) {
            return self->ptr;
        }
        if (*self->ptr == c) {
            u8 *ptr = self->ptr;
            lex_next(self);
            return ptr;
        }
        lex_next(self);
    }
}

static tok lex_scan_doc(lex *self) {
    if (!lex_expect_char(self, '#')) {
        return tok_from_tag(TOK_ERR);
    }

    if (!lex_skip(self)) {
        return tok_new(TOK_DOC, slice_empty());
    }

    u8 *start = self->ptr;
    u8 *end = lex_skip_until(self, '\n');
    return tok_new(TOK_DOC, SLICE(start, end));
}

static tok lex_scan_comment(lex *self) {
    if (!lex_skip(self)) {
        return tok_new(TOK_COMMENT, slice_empty());
    }

    u8 *start = self->ptr;
    u8 *end = lex_skip_until(self, '\n');
    return tok_new(TOK_COMMENT, SLICE(start, end));
}

static tok lex_scan_attr(lex *self) {
    if (!lex_skip(self)) {
        return tok_new(TOK_ATTR, slice_empty());
    }

    u8 *start = self->ptr;
    u8 *end = lex_skip_until(self, '\n');
    return tok_new(TOK_ATTR, SLICE(start, end));
}

static tok lex_scan(lex *self) {
    if (!lex_skip(self)) {
        return tok_from_tag(TOK_END);
    }

    u8 c = *self->ptr;
    lex_next(self);

    switch (c) {
    case '\n':
        return tok_from_tag(TOK_NL);

    case '(':
        return tok_from_tag(TOK_LPAR);

    case ')':
        return tok_from_tag(TOK_RPAR);

    case '{':
        return tok_from_tag(TOK_LBRC);

    case '}':
        return tok_from_tag(TOK_RBRC);

    case '[':
        return tok_from_tag(TOK_LSBR);

    case ']':
        return tok_from_tag(TOK_RSBR);

    case ',':
        return tok_from_tag(TOK_COMMA);

    case '#': {
        lex state = *self;
        tok t = lex_scan_doc(self);
        if (t.tag == TOK_ERR) {
            *self = state;
            t = lex_scan_comment(self);
        }

        return t;
    }

    case '@':
        return lex_scan_attr(self);

    default:
        return tok_from_tag(TOK_ERR);
    }
}
