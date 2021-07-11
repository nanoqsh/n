#include "../test.h"

#include "../../src/tools/lex.h"
#include "../../src/tools/tok.h"
#include <stdio.h>

static void scan() {
    const char *src = "  \t\n\n   \n";
    lex l = lex_new(slice_from_str(src));
    tok t;
    while ((t = lex_scan(&l)).tag != TOK_END) {
        tok_print(&t, stdout);
        printf(" %zu %zu", lex_ln(&l), lex_col(&l));
        puts("");
    }
    puts("END");
}

void test_lex() { scan(); }
