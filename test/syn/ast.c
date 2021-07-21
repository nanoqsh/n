#include "../test.h"

#include "../../src/syn/ast.h"

static void new_drop() {
    an val_ni = an_new_val(an_val_new_name(BOX_STR("ni")));
    an val_int = an_new_val(an_val_new_int(12));
    an val_x = an_new_val(an_val_new_name(BOX_STR("x")));
    an log_and = an_new_log(an_log_new_and(BOX(&val_ni), BOX(&val_int)));
    an log_or = an_new_log(an_log_new_or(BOX(&log_and), BOX(&val_x)));
    an_print(&log_or, false);
    puts("");
    an_print(&log_or, true);
    an_drop(&log_or);
}

void test_ast() { new_drop(); }
