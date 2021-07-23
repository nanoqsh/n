#include "../test.h"

#include "../../src/syn/ast.h"

static void new_drop() {
    an log;
    {
        an lhs = an_new_val(an_val_new_name(BOX_STR("ni")));
        an rhs = an_new_val(an_val_new_int(12));
        log = an_new_log(an_log_new_and(BOX(&lhs), BOX(&rhs)));
    }

    an pro;
    {
        an lhs = an_new_val(an_val_new_int(2));
        an rhs = an_new_val(an_val_new_int(3));
        pro = an_new_pro(an_pro_new_mul(BOX(&lhs), BOX(&rhs)));
    }

    an sum;
    {
        an lhs = an_new_val(an_val_new_int(0));
        an rhs = an_new_val(an_val_new_int(4));
        sum = an_new_sum(an_sum_new_add(BOX(&lhs), BOX(&rhs)));
    }

    an cmp = an_new_cmp(an_cmp_new_eq(BOX(&pro), BOX(&sum)));
    cmp = an_new_cmp(an_cmp_new_ne(BOX(&cmp), BOX(&log)));

    an_print(&cmp, false);
    puts("");
    an_print(&cmp, true);
    an_drop(&cmp);
}

void test_ast() { new_drop(); }
