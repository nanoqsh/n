#include "../test.h"

#include "../../src/tools/lex.h"
#include "../../src/tools/tok.h"
#include <stdio.h>

static void scan() {
    const char *src = "## doca\n\
    ()#comment\n\
    \"hello\" 'q'";

    lex l = lex_new(SLICE_STR(src));
    tok t;
    while (true) {
        t = lex_scan(&l);
        if (t.tag == TOK_END) {
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
