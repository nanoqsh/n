#include "../test.h"

#include "../../src/tools/ast.h"

static void new_drop() {
    tok ts[] = {
        tok_new(TOK_DEC, SLICE_STR("10")),
        tok_new(TOK_BIN, SLICE_STR("0b1")),
        tok_new(TOK_OCT, SLICE_STR("0o7")),
        tok_new(TOK_HEX, SLICE_STR("0xF")),
        tok_new(TOK_FLT, SLICE_STR("1.5")),
        tok_new(TOK_STR, SLICE_STR("hello")),
        tok_new(TOK_CHR, SLICE_STR("a")),
        tok_new(TOK_BOOL, SLICE_STR("true")),
        tok_new(TOK_NAME, SLICE_STR("meow")),
    };

    for (word i = 0; i < ARRAY_SIZE(ts); ++i) {
        ast a = ast_from_tok(ts + i);
        ast_print(&a, stdout);
        puts("");
        ast_drop(&a);
    }

    tok lt = tok_new(TOK_NAME, SLICE_STR("x"));
    tok rt = tok_new(TOK_DEC, SLICE_STR("10"));
    ast l = ast_from_tok(&lt);
    ast r = ast_from_tok(&rt);
    ast a = ast_from_pair(AST_ADD, l, r);
    ast_print(&a, stdout);
    puts("");
    ast_drop(&a);
}

void test_ast() { new_drop(); }
