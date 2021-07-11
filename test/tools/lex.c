#include "../test.h"

#include "../../src/tools/lex.h"
#include "../../src/tools/tok.h"

static void scan() {
    const char *src = "# hello\n#world\n#     !\n#";
    lex l = lex_new(slice_from_str(src));
    tok t;
    while ((t = lex_scan(&l)).tag != TOK_END) {
        tok_print(&t, stdout);
        puts("");
    }
    puts("END");
}

void test_lex() { scan(); }
