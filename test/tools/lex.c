#include "../test.h"

#include "../../src/tools/lex.h"
#include "../../src/tools/tok.h"
#include <stdio.h>

static void scan() {
    const char *src = "let x = 0\
    for i in 0 { 8 * x == 5 }\
    if x <= 6 { x / 2 } else { x % 2 }";

    lex l = lex_new(SLICE_STR(src));
    tok t;
    while (true) {
        lex_skip(&l);
        word ln = lex_ln(&l);
        word col = lex_col(&l);
        t = lex_scan(&l);
        if (t.tag == TOK_END) {
            break;
        }

        if (t.tag == TOK_ERR) {
            puts("ERR");
            break;
        }

        printf("%zu:%zu\t", ln, col);
        tok_print(&t, stdout);
        puts("");
    }
}

void test_lex() { scan(); }
