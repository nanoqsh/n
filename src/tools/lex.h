#pragma once

#include "../def.h"
#include "../lib/vec.h"
#include "tok.h"

static wbool is_bin(char c) { return BOOL_TO_WBOOL(c == '0' || c == '1' || c == '_'); }

static wbool is_oct(char c) { return BOOL_TO_WBOOL((c >= '0' && c <= '7') || c == '_'); }

static wbool is_dec(char c) { return BOOL_TO_WBOOL((c >= '0' && c <= '9') || c == '_'); }

static wbool is_hex(char c) {
    return BOOL_TO_WBOOL(is_dec(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

static wbool is_alp(char c) {
    return BOOL_TO_WBOOL((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

static wbool is_sym(char c) { return BOOL_TO_WBOOL(is_dec(c) || is_alp(c)); }

static wbool is_quote_or_backslash(char c) { return BOOL_TO_WBOOL(c == '"' || c == '\\'); }

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

static wbool _exp_not_at(lex *self, const char *c) {
    if (lex_go(self) && lex_at(self) != *c) {
        lex_next(self);
        return NONNULL;
    }

    return NULL;
}

static wbool _exp_pred(lex *self, wbool (*pred)(char)) {
    if (lex_go(self) && pred(lex_at(self))) {
        lex_next(self);
        return NONNULL;
    }

    return NULL;
}

static wbool _exp_pred_not(lex *self, wbool (*pred)(char)) {
    if (lex_go(self) && !pred(lex_at(self))) {
        lex_next(self);
        return NONNULL;
    }

    return NULL;
}

static tok lex_scan_number(lex *self, u8 *start) {
    hof p = HOF_WITH(_exp_pred, is_dec);
    u8 *end = lex_expect_while(self, p);
    _lex_state state = self->state;
    if (lex_expect_char(self, '.')) {
        if (lex_expect_char(self, '.') || lex_expect_char(self, '|')) {
            self->state = state;
            return tok_new(TOK_DEC, SLICE(start, end));
        }

        end = lex_expect_while(self, p);
        return tok_new(TOK_FLT, SLICE(start, end));
    }
    return tok_new(TOK_DEC, SLICE(start, end));
}

static tok lex_scan(lex *self) {
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

    u8 *start;
    u8 *end;

    if (lex_expect_char(self, '#')) {
        tok_tag tag = lex_expect_char(self, '#') ? TOK_DOC : TOK_COMMENT;
        lex_skip(self);
        start = lex_ptr(self);
        end = lex_expect_while(self, HOF_WITH(_exp_not_at, &nl));
        lex_go(self) && lex_next(self);
        return tok_new(tag, SLICE(start, end));
    }

    if (lex_expect_char(self, '@')) {
        lex_skip(self);
        start = lex_ptr(self);
        end = lex_expect_while(self, HOF_WITH(_exp_not_at, &nl));
        lex_go(self) && lex_next(self);
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
            return lex_scan_number(self, start);
        }

        if (!lex_expect_pred(self, p)) {
            return tok_from_tag(TOK_ERR);
        }

        end = lex_expect_while(self, p);
        return tok_new(tag, SLICE(start, end));
    }

    if (lex_expect_pred(self, p)) {
        return lex_scan_number(self, start);
    }

    char quote = '"';
    if (lex_expect_char(self, quote)) {
        start = lex_ptr(self);
        while (true) {
            end = lex_expect_while(self, HOF_WITH(_exp_pred_not, is_quote_or_backslash));
            if (!lex_go(self)) {
                return tok_from_tag(TOK_ERR);
            }

            if (!lex_expect_char(self, '\\')) {
                break;
            }

            lex_next(self);
        }

        lex_go(self) && lex_next(self);
        return tok_new(TOK_STR, SLICE(start, end));
    }

    if (lex_expect_char(self, '\'')) {
        start = lex_ptr(self);
        if (lex_expect_char(self, '\\')) {
            lex_go(self) && lex_next(self);
            if ((end = lex_expect_char(self, '\''))) {
                return tok_new(TOK_CHR, SLICE(start, end));
            }

            return tok_from_tag(TOK_ERR);
        } else if (lex_expect_char(self, '\'')) {
            return tok_from_tag(TOK_ERR);
        } else if (lex_go(self)) {
            lex_next(self);
            end = lex_ptr(self);
            if (lex_expect_char(self, '\'')) {
                return tok_new(TOK_CHR, SLICE(start, end));
            }

            return tok_from_tag(TOK_ERR);
        }

        return tok_from_tag(TOK_ERR);
    }

    if (lex_expect_pred(self, HOF_WITH(_exp_pred, is_alp))) {
        end = lex_expect_while(self, HOF_WITH(_exp_pred, is_sym));
        slice name = SLICE(start, end);

        if (slice_cmp(name, SLICE_STR("tru"))) {
            return tok_from_tag(TOK_TRU);
        }

        if (slice_cmp(name, SLICE_STR("fal"))) {
            return tok_from_tag(TOK_FAL);
        }

        if (slice_cmp(name, SLICE_STR("let"))) {
            return tok_new(TOK_LET, name);
        }

        if (slice_cmp(name, SLICE_STR("type"))) {
            return tok_new(TOK_TYPE, name);
        }

        if (slice_cmp(name, SLICE_STR("ret"))) {
            return tok_new(TOK_RET, name);
        }

        if (slice_cmp(name, SLICE_STR("if"))) {
            return tok_new(TOK_IF, name);
        }

        if (slice_cmp(name, SLICE_STR("el"))) {
            return tok_new(TOK_EL, name);
        }

        if (slice_cmp(name, SLICE_STR("when"))) {
            return tok_new(TOK_WHEN, name);
        }

        if (slice_cmp(name, SLICE_STR("for"))) {
            return tok_new(TOK_FOR, name);
        }

        if (slice_cmp(name, SLICE_STR("in"))) {
            return tok_new(TOK_IN, name);
        }

        if (slice_cmp(name, SLICE_STR("go"))) {
            return tok_new(TOK_GO, name);
        }

        if (slice_cmp(name, SLICE_STR("_"))) {
            return tok_new(TOK_UNDER, name);
        }

        return tok_new(TOK_NAME, name);
    }

    if (lex_expect_char(self, '~')) {
        return tok_from_tag(TOK_TILDA);
    }

    if (lex_expect_char(self, '.')) {
        if (lex_expect_char(self, '.')) {
            return tok_from_tag(TOK_RNG_CC);
        }

        if (lex_expect_char(self, '|')) {
            return tok_from_tag(TOK_RNG_CO);
        }
        return tok_from_tag(TOK_DOT);
    }

    if (lex_expect_char(self, ',')) {
        return tok_from_tag(TOK_COMMA);
    }

    if (lex_expect_char(self, ':')) {
        return tok_from_tag(TOK_COLON);
    }

    if (lex_expect_char(self, '=')) {
        if (lex_expect_char(self, '=')) {
            return tok_from_tag(TOK_EQ);
        }
        return tok_from_tag(TOK_ASSIGN);
    }

    if (lex_expect_char(self, '+')) {
        if (lex_expect_char(self, '=')) {
            return tok_from_tag(TOK_ADD_ASSIGN);
        }
        return tok_from_tag(TOK_ADD);
    }

    if (lex_expect_char(self, '-')) {
        if (lex_expect_char(self, '=')) {
            return tok_from_tag(TOK_SUB_ASSIGN);
        }
        return tok_from_tag(TOK_SUB);
    }

    if (lex_expect_char(self, '*')) {
        if (lex_expect_char(self, '=')) {
            return tok_from_tag(TOK_MUL_ASSIGN);
        }
        return tok_from_tag(TOK_MUL);
    }

    if (lex_expect_char(self, '/')) {
        if (lex_expect_char(self, '=')) {
            return tok_from_tag(TOK_DIV_ASSIGN);
        }
        return tok_from_tag(TOK_DIV);
    }

    if (lex_expect_char(self, '%')) {
        if (lex_expect_char(self, '=')) {
            return tok_from_tag(TOK_REM_ASSIGN);
        }
        return tok_from_tag(TOK_REM);
    }

    if (lex_expect_char(self, '!')) {
        if (lex_expect_char(self, '=')) {
            return tok_from_tag(TOK_NEQ);
        }
        return tok_from_tag(TOK_NOT);
    }

    if (lex_expect_char(self, '<')) {
        if (lex_expect_char(self, '=')) {
            return tok_from_tag(TOK_LE);
        }
        return tok_from_tag(TOK_LT);
    }

    if (lex_expect_char(self, '>')) {
        if (lex_expect_char(self, '=')) {
            return tok_from_tag(TOK_GE);
        }
        return tok_from_tag(TOK_GT);
    }

    if (lex_expect_char(self, '&')) {
        if (lex_expect_char(self, '=')) {
            return tok_from_tag(TOK_AND_ASSIGN);
        }
        return tok_from_tag(TOK_AND);
    }

    if (lex_expect_char(self, '|')) {
        if (lex_expect_char(self, '=')) {
            return tok_from_tag(TOK_OR_ASSIGN);
        }

        if (lex_expect_char(self, '.')) {
            return tok_from_tag(TOK_RNG_OC);
        }

        if (lex_expect_char(self, '|')) {
            return tok_from_tag(TOK_RNG_OO);
        }
        return tok_from_tag(TOK_OR);
    }

    if (lex_expect_char(self, '^')) {
        if (lex_expect_char(self, '=')) {
            return tok_from_tag(TOK_XOR_ASSIGN);
        }
        return tok_from_tag(TOK_XOR);
    }

    return tok_from_tag(TOK_ERR);
}
