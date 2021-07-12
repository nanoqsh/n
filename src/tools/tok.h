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
