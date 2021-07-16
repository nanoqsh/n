#pragma once

#include "../def.h"
#include <stdio.h>

typedef enum {
    TOK_NL,
    TOK_LPAR,
    TOK_RPAR,
    TOK_LBRC,
    TOK_RBRC,
    TOK_LSBR,
    TOK_RSBR,
    TOK_DOC,
    TOK_COMMENT,
    TOK_ATTR,
    TOK_DEC,
    TOK_BIN,
    TOK_OCT,
    TOK_HEX,
    TOK_FLT,
    TOK_STR,
    TOK_CHR,
    TOK_BOOL,
    TOK_NAME,
    TOK_LET,
    TOK_RET,
    TOK_IF,
    TOK_ELSE,
    TOK_FOR,
    TOK_IN,
    TOK_COMMA,
    TOK_COLON,
    TOK_ASSIGN,
    TOK_ADD,
    TOK_SUB,
    TOK_MUL,
    TOK_DIV,
    TOK_REM,
    TOK_ADD_ASSIGN,
    TOK_SUB_ASSIGN,
    TOK_MUL_ASSIGN,
    TOK_DIV_ASSIGN,
    TOK_REM_ASSIGN,
    TOK_EQ,
    TOK_NEQ,
    TOK_LT,
    TOK_GT,
    TOK_LE,
    TOK_GE,
    TOK_NOT,
    TOK_AND,
    TOK_OR,
    TOK_XOR,
    TOK_AND_ASSIGN,
    TOK_OR_ASSIGN,
    TOK_XOR_ASSIGN,
    TOK_END,
    TOK_ERR,
} tok_tag;

typedef struct {
    tok_tag tag;
    slice str;
} tok;

static tok tok_new(tok_tag tag, slice str) {
    return (tok){
        .tag = tag,
        .str = str,
    };
}

static tok tok_from_tag(tok_tag tag) { return tok_new(tag, slice_empty()); }

static bool _tok_str_to_num(slice str, u8 base, u64 *out) {
    u64 base_pow = 1;
    u8 *p = str.end;
    while (p != str.start) {
        u8 c = *--p;
        u8 d;
        if (c >= '0' && c <= '9') {
            d = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            d = c - 'A' + 10;
        } else {
            UNREACHABLE;
        }

        u64 n = d * base_pow;
        if (n / base_pow != d) {
            return false;
        }

        *out += n;
        if (*out < n) {
            return false;
        }

        u64 pow = base_pow * base;
        if (pow / base != base_pow) {
            return p == str.start;
        }
        base_pow = pow;
    }

    return true;
}

static bool tok_to_num(const tok *self, u64 *out) {
    u8 base;
    switch (self->tag) {
    case TOK_DEC:
        return _tok_str_to_num(self->str, 10, out);

    case TOK_BIN:
        base = 2;
        break;

    case TOK_OCT:
        base = 8;
        break;

    case TOK_HEX:
        base = 16;
        break;

    default:
        return false;
    }

    word len = slice_len_bytes(self->str);
    slice str = slice_subslice(self->str, RANGE(2, len), sizeof(u8));
    return _tok_str_to_num(str, base, out);
}

static void tok_print(const tok *self, FILE *file) {
    switch (self->tag) {
    case TOK_NL:
        fprintf(file, "%s", "\\n");
        break;

    case TOK_LPAR:
        fputc('(', file);
        break;

    case TOK_RPAR:
        fputc(')', file);
        break;

    case TOK_LBRC:
        fputc('{', file);
        break;

    case TOK_RBRC:
        fputc('}', file);
        break;

    case TOK_LSBR:
        fputc('[', file);
        break;

    case TOK_RSBR:
        fputc(']', file);
        break;

    case TOK_DOC:
        fprintf(file, "%s", "## ");
        slice_print(self->str, file);
        break;

    case TOK_COMMENT:
        fprintf(file, "%s", "# ");
        slice_print(self->str, file);
        break;

    case TOK_ATTR:
        fprintf(file, "%s", "@ ");
        slice_print(self->str, file);
        break;

    case TOK_DEC:
        fprintf(file, "%s", "DEC=");
        slice_print(self->str, file);
        break;

    case TOK_BIN:
        fprintf(file, "%s", "BIN=");
        slice_print(self->str, file);
        break;

    case TOK_OCT:
        fprintf(file, "%s", "OCT=");
        slice_print(self->str, file);
        break;

    case TOK_HEX:
        fprintf(file, "%s", "HEX=");
        slice_print(self->str, file);
        break;

    case TOK_FLT:
        fprintf(file, "%s", "FLT=");
        slice_print(self->str, file);
        break;

    case TOK_BOOL:
        slice_print(self->str, file);
        break;

    case TOK_STR:
        fputc('"', file);
        slice_print(self->str, file);
        fputc('"', file);
        break;

    case TOK_CHR:
        fputc('\'', file);
        slice_print(self->str, file);
        fputc('\'', file);
        break;

    case TOK_NAME:
        fprintf(file, "%s", "NAME=");
        slice_print(self->str, file);
        break;

    case TOK_LET:
        fprintf(file, "%s", "let");
        break;

    case TOK_RET:
        fprintf(file, "%s", "ret");
        break;

    case TOK_IF:
        fprintf(file, "%s", "if");
        break;

    case TOK_ELSE:
        fprintf(file, "%s", "else");
        break;

    case TOK_FOR:
        fprintf(file, "%s", "for");
        break;

    case TOK_IN:
        fprintf(file, "%s", "in");
        break;

    case TOK_COMMA:
        fputc(',', file);
        break;

    case TOK_COLON:
        fputc(':', file);
        break;

    case TOK_ASSIGN:
        fputc('=', file);
        break;

    case TOK_ADD:
        fputc('+', file);
        break;

    case TOK_SUB:
        fputc('-', file);
        break;

    case TOK_MUL:
        fputc('*', file);
        break;

    case TOK_DIV:
        fputc('/', file);
        break;

    case TOK_REM:
        fputc('%', file);
        break;

    case TOK_ADD_ASSIGN:
        fprintf(file, "%s", "+=");
        break;

    case TOK_SUB_ASSIGN:
        fprintf(file, "%s", "-=");
        break;

    case TOK_MUL_ASSIGN:
        fprintf(file, "%s", "*=");
        break;

    case TOK_DIV_ASSIGN:
        fprintf(file, "%s", "/=");
        break;

    case TOK_REM_ASSIGN:
        fprintf(file, "%s", "%=");
        break;

    case TOK_EQ:
        fprintf(file, "%s", "==");
        break;

    case TOK_NEQ:
        fprintf(file, "%s", "!=");
        break;

    case TOK_LT:
        fputc('<', file);
        break;

    case TOK_GT:
        fputc('>', file);
        break;

    case TOK_LE:
        fprintf(file, "%s", "<=");
        break;

    case TOK_GE:
        fprintf(file, "%s", ">=");
        break;

    case TOK_NOT:
        fputc('!', file);
        break;

    case TOK_AND:
        fputc('&', file);
        break;

    case TOK_OR:
        fputc('|', file);
        break;

    case TOK_XOR:
        fputc('^', file);
        break;

    case TOK_AND_ASSIGN:
        fprintf(file, "%s", "&=");
        break;

    case TOK_OR_ASSIGN:
        fprintf(file, "%s", "|=");
        break;

    case TOK_XOR_ASSIGN:
        fprintf(file, "%s", "^=");
        break;

    case TOK_END:
        fprintf(file, "%s", "END");
        break;

    case TOK_ERR:
        fprintf(file, "%s", "ERR");
        break;

    default:
        UNREACHABLE;
    }
}
