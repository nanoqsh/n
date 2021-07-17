#include "../test.h"

#include "../../src/tools/ast.h"

static void new_drop() {
    ast a;
    {
        tok lt = tok_new(TOK_NAME, SLICE_STR("x"));
        tok rt = tok_new(TOK_DEC, SLICE_STR("10"));
        ast l = ast_from_tok(&lt);
        ast r = ast_from_tok(&rt);
        a = ast_from_pair(AST_ADD, l, r);
    }

    ast m;
    {
        tok lt = tok_new(TOK_STR, SLICE_STR("ni"));
        tok rt = tok_new(TOK_DEC, SLICE_STR("5"));
        ast l = ast_from_tok(&lt);
        ast r = ast_from_tok(&rt);
        m = ast_from_pair(AST_MUL, l, r);
    }

    ast d;
    {
        tok lt = tok_new(TOK_BOOL, SLICE_STR("true"));
        tok rt = tok_new(TOK_DEC, SLICE_STR("7"));
        ast l = ast_from_tok(&lt);
        ast r = ast_from_tok(&rt);
        d = ast_from_pair(AST_SUB, l, r);
    }

    ast e = ast_from_pair(AST_DIV, a, ast_from_pair(AST_REM, m, d));
    ast_print(&e, stdout, false);
    puts("");
    ast_print(&e, stdout, true);
    puts("");
    ast_drop(&e);
}

void test_ast() { new_drop(); }
