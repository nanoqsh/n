#include "../test.h"

#include "../../src/tools/lex.h"
#include "../../src/tools/tok.h"
#include <stdio.h>

static void scan() {
    const char *src = "[] ## doca\n\
    (,)#comment\n\
    @ attr\n\
    {0b101_110name0b0}\n\
    ";

    lex l = lex_new(slice_from_str(src));
    tok t;
    while (true) {
        t = lex_scan(&l);
        if (t.tag == TOK_END) {
            puts("END");
            break;
        }

        if (t.tag == TOK_ERR) {
            puts("ERR");
            break;
        }

        printf("%zu:%zu\t", lex_ln(&l), lex_col(&l));
        tok_print(&t, stdout);
        puts("");
    }
}

void test_lex() { scan(); }
