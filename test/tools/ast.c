#include "../test.h"

#include "../../src/tools/ast.h"

static void new_drop() {
    ast a;
    {
        tok lt = tok_new(TOK_NAME, SLICE_STR("x"));
        tok rt = tok_new(TOK_DEC, SLICE_STR("10"));
        ast l = ast_single(AST_NEG, ast_from_tok(&lt));
        ast r = ast_from_tok(&rt);
        a = ast_binary(AST_AND, l, r);
    }

    ast m;
    {
        tok lt = tok_new(TOK_STR, SLICE_STR("ni"));
        tok rt = tok_new(TOK_DEC, SLICE_STR("5"));
        ast l = ast_from_tok(&lt);
        ast r = ast_from_tok(&rt);
        m = ast_binary(AST_OR, l, r);
    }

    ast d;
    {
        tok lt = tok_new(TOK_BOOL, SLICE_STR("true"));
        tok rt = tok_new(TOK_DEC, SLICE_STR("7"));
        ast l = ast_single(AST_NOT, ast_from_tok(&lt));
        ast r = ast_from_tok(&rt);
        d = ast_binary(AST_XOR, l, r);
    }

    ast i;
    {
        tok lt = tok_new(TOK_FLT, SLICE_STR("0.5"));
        tok rt = tok_new(TOK_DEC, SLICE_STR("7"));
        ast l = ast_from_tok(&lt);
        ast r = ast_from_tok(&rt);
        ast s = ast_binary(AST_ADD, l, r);
        l = ast_from_tok(&lt);
        r = ast_from_tok(&rt);
        ast b[] = {s, l, r};
        i = ast_list(AST_TUPLE, SLICE_FROM_ARRAY(b));
    }

    ast b[] = {a, m, i, d};
    ast e = ast_list(AST_BLOCK, SLICE_FROM_ARRAY(b));
    ast_print(&e, stdout, false);
    puts("");
    ast_print(&e, stdout, true);
    puts("");
    ast_drop(&e);
}

void test_ast() { new_drop(); }
