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

    ast d;
    {
        tok lt = tok_from_tag(TOK_TRU);
        tok rt = tok_new(TOK_DEC, SLICE_STR("7"));
        ast l = ast_single(AST_NOT, ast_from_tok(&lt));
        ast r = ast_from_tok(&rt);
        d = ast_binary(AST_XOR, l, r);
    }

    ast p;
    {
        tok name = tok_new(TOK_NAME, SLICE_STR("Hi"));
        ast n = ast_from_tok(&name);
        tok a_name = tok_new(TOK_NAME, SLICE_STR("a"));
        tok b_name = tok_new(TOK_NAME, SLICE_STR("b"));
        ast a = ast_from_tok(&a_name);
        ast b = ast_from_tok(&b_name);
        ast pats[] = {a, b};
        an_tuple_pat tup = an_tuple_pat_new(true, BOX_FROM_ARRAY(pats));
        ast t = ast_from_box(AST_TUPLE_PAT, BOX(&tup));
        an_pat pat = an_pat_new_tuple(BOX(&n), BOX(&t));
        p = ast_from_box(AST_PAT, BOX(&pat));
    }

    ast b[] = {a, d, p};
    ast e = ast_list(AST_BLOCK, SLICE_FROM_ARRAY(b));
    ast_print(&e, stdout, false);
    puts("");
    ast_print(&e, stdout, true);
    puts("");
    ast_drop(&e);
}

void test_ast() { new_drop(); }
