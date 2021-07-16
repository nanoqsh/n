#pragma once

#include "../def.h"
#include "../tools/tok.h"

typedef enum {
    AST_NUM,
    AST_FLT,
    AST_STR,
    AST_CHR,
    AST_BOOL,
    AST_NAME,
    AST_ERR,
} ast_tag;

typedef struct {
    ast_tag tag;
    box data;
} ast;

static ast ast_from_tag(ast_tag tag) {
    return (ast){
        .tag = tag,
        .data = box_empty(),
    };
}

static ast ast_from_tok(const tok *t) {
    ast_tag tag;
    switch (t->tag) {
    case TOK_NAME:
        tag = AST_NAME;
        break;

    default:
        return ast_from_tag(AST_ERR);
    }

    return (ast){
        .tag = tag,
        .data = box_from_slice(t->str),
    };
}

static ast ast_drop(const ast *self) { box_drop(self->data); }
