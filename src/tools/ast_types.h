#pragma once

#include "../def.h"

typedef enum {
    AST_NONE,
    // expr
    // leaf
    AST_NUM,
    AST_FLT,
    AST_STR,
    AST_CHR,
    AST_BOOL,
    AST_NAME,
    // end leaf
    AST_NEG,
    AST_NOT,
    // binary
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_REM,
    AST_EQ,
    AST_NEQ,
    AST_LT,
    AST_GT,
    AST_LE,
    AST_GE,
    AST_AND,
    AST_OR,
    AST_XOR,
    // end binary
    AST_TUPLE,
    AST_BLOCK,
    // end expr
    AST_PAT,
    AST_TUPLE_PAT,
    AST_DECL,
} ast_tag;

bool ast_tag_is_leaf(ast_tag self) { return self >= AST_NUM && self <= AST_NAME; }

bool ast_tag_is_expr(ast_tag self) { return self >= AST_NUM && self <= AST_BLOCK; }

bool ast_tag_is_binary(ast_tag self) { return self >= AST_ADD && self <= AST_XOR; }

typedef struct {
    ast_tag tag;
    union {
        box x;
        u8 b1;
        u32 b4;
        u64 b8;
    } data;
} ast;

static ast ast_none() {
    return (ast){
        .tag = AST_NONE,
    };
}

static ast ast_from_box(ast_tag tag, box x) {
    return (ast){
        .tag = tag,
        .data.x = x,
    };
}

static ast ast_single(ast_tag tag, ast node) {
    DEBUG_ASSERT(!ast_tag_is_binary(tag));
    DEBUG_ASSERT(ast_tag_is_expr(node.tag));
    return ast_from_box(tag, BOX(&node));
}

static ast ast_binary(ast_tag tag, ast left, ast right) {
    DEBUG_ASSERT(ast_tag_is_binary(tag));
    DEBUG_ASSERT(ast_tag_is_expr(left.tag));
    DEBUG_ASSERT(ast_tag_is_expr(right.tag));

    ast pair[] = {left, right};
    return (ast){
        .tag = tag,
        .data.x = BOX_FROM_ARRAY(pair),
    };
}

static ast ast_from_list(ast_tag tag, slice list) {
    DEBUG_ASSERT(tag == AST_BLOCK || tag == AST_TUPLE);
    return ast_from_box(tag, box_from_slice(list));
}

static void ast_drop(const ast *);
