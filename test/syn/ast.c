#include "../test.h"

#include "../../src/syn/ast.h"

static void new_drop() {
    an_expr and;
    {
        box tail[] = {BOX_STR("foo"), BOX_STR("bar")};
        an_expr lhs =
            an_expr_new_val(an_val_new_path(an_path_new(BOX_STR("ni"), BOX_FROM_ARRAY(tail))));
        an_expr rhs = an_expr_new_val(an_val_new_int(12));
        and = an_expr_new_and(&lhs, &rhs);
    }

    an_expr mul;
    {
        an_expr lhs = an_expr_new_val(an_val_new_int(2));
        an_expr rhs = an_expr_new_val(an_val_new_int(3));
        mul = an_expr_new_mul(&lhs, &rhs);
    }

    an_expr add;
    {
        an_expr lhs = an_expr_new_val(an_val_new_int(0));
        an_expr rhs = an_expr_new_val(an_val_new_int(4));
        add = an_expr_new_add(&lhs, &rhs);
    }

    an_expr eq = an_expr_new_eq(&mul, &add);
    eq = an_expr_new_ne(&eq, &and);

    an_expr_print(&eq, false);
    puts("");
    an_expr_print(&eq, true);
    an_expr_drop(&eq);
}

void test_ast() { new_drop(); }
