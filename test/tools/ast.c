#include "../test.h"

#include "../../src/tools/ast.h"

static void new_drop() {
    ast a;
    {
        tok lt = tok_new(TOK_NAME, SLICE_STR("x"));
        tok rt = tok_new(TOK_DEC, SLICE_STR("10"));
        ast l = ast_single(AST_NEG, ast_from_tok(&lt));
        ast r = ast_from_tok(&rt);
        ast and = ast_binary(AST_AND, l, r);

        box name = box_from_slice(SLICE_STR("val"));
        an_decl_pat pat = an_decl_pat_new_name(false, name);

        an_decl decl = an_decl_new(pat, ast_none(), and);
        a = ast_from_box(AST_DECL, BOX(&decl));
    }

    ast p;
    {
        box name = box_from_slice(SLICE_STR("Data"));
        tok a_name = tok_new(TOK_NAME, SLICE_STR("a"));
        tok b_name = tok_new(TOK_NAME, SLICE_STR("b"));
        ast a = ast_from_tok(&a_name);
        ast b = ast_from_tok(&b_name);
        ast pats[] = {a, b};
        an_tuple_pat tup = an_tuple_pat_new(false, BOX_FROM_ARRAY(pats));
        ast t = ast_from_box(AST_TUPLE_PAT, BOX(&tup));
        tok c_name = tok_new(TOK_NAME, SLICE_STR("c"));
        ast c = ast_from_tok(&c_name);
        an_decl_pat under = an_decl_pat_new_under();
        ast u = ast_from_box(AST_DECL_PAT, BOX(&under));
        ast pats2[] = {t, c, u};
        an_tuple_pat tup2 = an_tuple_pat_new(true, BOX_FROM_ARRAY(pats2));
        an_decl_pat pat = an_decl_pat_new_tuple(name, tup2);

        tok tt = tok_new(TOK_NAME, SLICE_STR("Data"));
        ast typ = ast_from_tok(&tt);

        ast d;
        {
            tok lt = tok_from_tag(TOK_TRU);
            tok rt = tok_new(TOK_DEC, SLICE_STR("7"));
            ast l = ast_from_tok(&lt);
            ast r = ast_from_tok(&rt);
            d = ast_binary(AST_XOR, l, r);
        }

        an_decl decl = an_decl_new(pat, typ, d);
        p = ast_from_box(AST_DECL, BOX(&decl));
    }

    ast b[] = {a, p};
    ast e = ast_from_list(AST_BLOCK, SLICE_FROM_ARRAY(b));
    ast_print(&e, stdout, false);
    puts("");
    ast_print(&e, stdout, true);
    puts("");
    ast_drop(&e);
}

void test_ast() { new_drop(); }
