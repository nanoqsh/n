#pragma once

#include "../def.h"
#include "ast_printer.h"

typedef struct an an;
static bool an_is_expr(const an *);
static void an_drop(const an *);

typedef struct {
    bool tilda;
    box name;
} an_var;

// name: str
static an_var an_var_new(bool tilda, box name) {
    return (an_var){
        .tilda = tilda,
        .name = name,
    };
}

static void an_var_drop(const an_var *self) { box_drop(self->name); }

static void an_var_print(const an_var *self, ast_printer *printer) {
    ast_printer_print_node(printer);
    if (self->tilda) {
        ast_printer_print_string(printer, "~");
    }
    ast_printer_print_slice(printer, box_to_slice(self->name));
    ast_printer_nl(printer);
}

typedef enum {
    AN_VAL__INT,
    AN_VAL__FLT,
    AN_VAL__STR,
    AN_VAL__CHR,
    AN_VAL__BOOL,
    AN_VAL__NAME,
} an_val_tag;

typedef struct {
    an_val_tag tag;
    union {
        box box;
        u8 u8;
        u32 u32;
        u64 u64;
    } val;
} an_val;

static an_val an_val_new_int(u64 val) {
    return (an_val){
        .tag = AN_VAL__INT,
        .val.u64 = val,
    };
}

// val: str
static an_val an_val_new_flt(box val) {
    return (an_val){
        .tag = AN_VAL__FLT,
        .val.box = val,
    };
}

// val: str
static an_val an_val_new_str(box val) {
    return (an_val){
        .tag = AN_VAL__STR,
        .val.box = val,
    };
}

static an_val an_val_new_chr(u32 val) {
    return (an_val){
        .tag = AN_VAL__CHR,
        .val.u32 = val,
    };
}

static an_val an_val_new_bool(bool val) {
    return (an_val){
        .tag = AN_VAL__BOOL,
        .val.u8 = val,
    };
}

// val: str
static an_val an_val_new_name(box val) {
    return (an_val){
        .tag = AN_VAL__NAME,
        .val.box = val,
    };
}

static void an_val_drop(const an_val *self) {
    switch (self->tag) {
    case AN_VAL__FLT:
    case AN_VAL__STR:
    case AN_VAL__NAME: {
        box_drop(self->val.box);
        break;
    }
    default:
        break;
    }
}

static void an_val_print(const an_val *self, ast_printer *printer) {
    ast_printer_print_node(printer);
    switch (self->tag) {
    case AN_VAL__INT: {
        ast_printer_print_u64(printer, self->val.u64);
        break;
    }
    case AN_VAL__FLT:
    case AN_VAL__NAME: {
        ast_printer_print_slice(printer, box_to_slice(self->val.box));
        break;
    }
    case AN_VAL__STR: {
        ast_printer_print_string(printer, "\"");
        ast_printer_print_slice(printer, box_to_slice(self->val.box));
        ast_printer_print_string(printer, "\"");
        break;
    }
    case AN_VAL__CHR: {
        ast_printer_print_string(printer, "'");
        ast_printer_print_char(printer, self->val.u32);
        ast_printer_print_string(printer, "'");
        break;
    }
    case AN_VAL__BOOL: {
        ast_printer_print_string(printer, self->val.u8 ? "tru" : "fal");
        break;
    }
    default:
        UNREACHABLE;
    }
    ast_printer_nl(printer);
}

typedef enum {
    AN_LOG__NOT,
    AN_LOG__AND,
    AN_LOG__OR,
    AN_LOG__XOR,
} an_log_tag;

typedef struct {
    an_log_tag tag;
    box lhs;
    box rhs;
} an_log;

// val: an (expr)
static an_log an_log_new_not(box val) {
    DEBUG_ASSERT(an_is_expr(box_data(val)));
    return (an_log){
        .tag = AN_LOG__NOT,
        .lhs = val,
    };
}

// lhs: an (expr)
// rhs: an (expr)
static an_log an_log_new_and(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_log){
        .tag = AN_LOG__AND,
        .lhs = lhs,
        .rhs = rhs,
    };
}

// lhs: an (expr)
// rhs: an (expr)
static an_log an_log_new_or(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_log){
        .tag = AN_LOG__OR,
        .lhs = lhs,
        .rhs = rhs,
    };
}

// lhs: an (expr)
// rhs: an (expr)
static an_log an_log_new_xor(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_log){
        .tag = AN_LOG__XOR,
        .lhs = lhs,
        .rhs = rhs,
    };
}

static void an_log_drop(const an_log *self) {
    switch (self->tag) {
    case AN_LOG__NOT: {
        an_drop(box_data(self->lhs));
        box_drop(self->lhs);
        break;
    }
    case AN_LOG__AND:
    case AN_LOG__OR:
    case AN_LOG__XOR: {
        an_drop(box_data(self->lhs));
        box_drop(self->lhs);
        an_drop(box_data(self->rhs));
        box_drop(self->rhs);
        break;
    }
    default:
        break;
    }
}

static void an_log_print(const an_log *self, ast_printer *printer) {
    const char *op;
    switch (self->tag) {
    case AN_LOG__NOT: {
        ast_printer_print_ast_unary(printer, box_data(self->lhs), "!");
        return;
    }
    case AN_LOG__AND: {
        op = "&";
        break;
    }
    case AN_LOG__OR: {
        op = "|";
        break;
    }
    case AN_LOG__XOR: {
        op = "^";
        break;
    }
    default:
        UNREACHABLE;
    }

    const an *ans[] = {box_data(self->lhs), box_data(self->rhs)};
    ast_printer_print_ast_binary(printer, ans, op);
}

typedef enum {
    AN_PRO__MUL,
    AN_PRO__DIV,
    AN_PRO__REM,
} an_pro_tag;

typedef struct {
    an_pro_tag tag;
    box lhs;
    box rhs;
} an_pro;

// lhs: an (expr)
// rhs: an (expr)
static an_pro an_pro_new_mul(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_pro){
        .tag = AN_PRO__MUL,
        .lhs = lhs,
        .rhs = rhs,
    };
}

// lhs: an (expr)
// rhs: an (expr)
static an_pro an_pro_new_div(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_pro){
        .tag = AN_PRO__DIV,
        .lhs = lhs,
        .rhs = rhs,
    };
}

// lhs: an (expr)
// rhs: an (expr)
static an_pro an_pro_new_rem(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_pro){
        .tag = AN_PRO__REM,
        .lhs = lhs,
        .rhs = rhs,
    };
}

static void an_pro_drop(const an_pro *self) {
    an_drop(box_data(self->lhs));
    box_drop(self->lhs);
    an_drop(box_data(self->rhs));
    box_drop(self->rhs);
}

static void an_pro_print(const an_pro *self, ast_printer *printer) {
    const char *op;
    switch (self->tag) {
    case AN_PRO__MUL: {
        op = "*";
        break;
    }
    case AN_PRO__DIV: {
        op = "/";
        break;
    }
    case AN_PRO__REM: {
        op = "%";
        break;
    }
    default:
        UNREACHABLE;
    }

    const an *ans[] = {box_data(self->lhs), box_data(self->rhs)};
    ast_printer_print_ast_binary(printer, ans, op);
}

typedef enum {
    AN_SUM__NEG,
    AN_SUM__ADD,
    AN_SUM__SUB,
} an_sum_tag;

typedef struct {
    an_sum_tag tag;
    box lhs;
    box rhs;
} an_sum;

// val: an (expr)
static an_sum an_sum_new_neg(box val) {
    DEBUG_ASSERT(an_is_expr(box_data(val)));
    return (an_sum){
        .tag = AN_SUM__NEG,
        .lhs = val,
    };
}

// lhs: an (expr)
// rhs: an (expr)
static an_sum an_sum_new_add(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_sum){
        .tag = AN_SUM__ADD,
        .lhs = lhs,
        .rhs = rhs,
    };
}

// lhs: an (expr)
// rhs: an (expr)
static an_sum an_sum_new_sub(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_sum){
        .tag = AN_SUM__SUB,
        .lhs = lhs,
        .rhs = rhs,
    };
}

static void an_sum_drop(const an_sum *self) {
    switch (self->tag) {
    case AN_SUM__NEG: {
        an_drop(box_data(self->lhs));
        box_drop(self->lhs);
        break;
    }
    case AN_SUM__ADD:
    case AN_SUM__SUB: {
        an_drop(box_data(self->lhs));
        box_drop(self->lhs);
        an_drop(box_data(self->rhs));
        box_drop(self->rhs);
        break;
    }
    default:
        break;
    }
}

static void an_sum_print(const an_sum *self, ast_printer *printer) {
    const char *op;
    switch (self->tag) {
    case AN_SUM__NEG: {
        ast_printer_print_ast_unary(printer, box_data(self->lhs), "-");
        return;
    }
    case AN_SUM__ADD: {
        op = "+";
        break;
    }
    case AN_SUM__SUB: {
        op = "-";
        break;
    }
    default:
        UNREACHABLE;
    }

    const an *ans[] = {box_data(self->lhs), box_data(self->rhs)};
    ast_printer_print_ast_binary(printer, ans, op);
}

typedef enum {
    AN_CMP__EQ,
    AN_CMP__NE,
    AN_CMP__LT,
    AN_CMP__GT,
    AN_CMP__LE,
    AN_CMP__GE,
} an_cmp_tag;

typedef struct {
    an_cmp_tag tag;
    box lhs;
    box rhs;
} an_cmp;

// lhs: an (expr)
// rhs: an (expr)
static an_cmp an_cmp_new_eq(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_cmp){
        .tag = AN_CMP__EQ,
        .lhs = lhs,
        .rhs = rhs,
    };
}

// lhs: an (expr)
// rhs: an (expr)
static an_cmp an_cmp_new_ne(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_cmp){
        .tag = AN_CMP__NE,
        .lhs = lhs,
        .rhs = rhs,
    };
}

// lhs: an (expr)
// rhs: an (expr)
static an_cmp an_cmp_new_lt(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_cmp){
        .tag = AN_CMP__LT,
        .lhs = lhs,
        .rhs = rhs,
    };
}

// lhs: an (expr)
// rhs: an (expr)
static an_cmp an_cmp_new_gt(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_cmp){
        .tag = AN_CMP__GT,
        .lhs = lhs,
        .rhs = rhs,
    };
}

// lhs: an (expr)
// rhs: an (expr)
static an_cmp an_cmp_new_le(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_cmp){
        .tag = AN_CMP__LE,
        .lhs = lhs,
        .rhs = rhs,
    };
}

// lhs: an (expr)
// rhs: an (expr)
static an_cmp an_cmp_new_ge(box lhs, box rhs) {
    DEBUG_ASSERT(an_is_expr(box_data(lhs)));
    DEBUG_ASSERT(an_is_expr(box_data(rhs)));
    return (an_cmp){
        .tag = AN_CMP__GE,
        .lhs = lhs,
        .rhs = rhs,
    };
}

static void an_cmp_drop(const an_cmp *self) {
    an_drop(box_data(self->lhs));
    box_drop(self->lhs);
    an_drop(box_data(self->rhs));
    box_drop(self->rhs);
}

static void an_cmp_print(const an_cmp *self, ast_printer *printer) {
    const char *op;
    switch (self->tag) {
    case AN_CMP__EQ: {
        op = "==";
        break;
    }
    case AN_CMP__NE: {
        op = "!=";
        break;
    }
    case AN_CMP__LT: {
        op = "<";
        break;
    }
    case AN_CMP__GT: {
        op = ">";
        break;
    }
    case AN_CMP__LE: {
        op = "<=";
        break;
    }
    case AN_CMP__GE: {
        op = ">=";
        break;
    }
    default:
        UNREACHABLE;
    }

    const an *ans[] = {box_data(self->lhs), box_data(self->rhs)};
    ast_printer_print_ast_binary(printer, ans, op);
}

typedef enum {
    AN__VAR,
    AN__VAL,
    AN__LOG,
    AN__PRO,
    AN__SUM,
    AN__CMP,
} an_tag;

typedef struct an {
    an_tag tag;
    union {
        an_var an_var;
        an_val an_val;
        an_log an_log;
        an_pro an_pro;
        an_sum an_sum;
        an_cmp an_cmp;
    } val;
} an;

static an an_new_var(an_var var) {
    return (an){
        .tag = AN__VAR,
        .val.an_var = var,
    };
}

static an an_new_val(an_val val) {
    return (an){
        .tag = AN__VAL,
        .val.an_val = val,
    };
}

static an an_new_log(an_log val) {
    return (an){
        .tag = AN__LOG,
        .val.an_log = val,
    };
}

static an an_new_pro(an_pro val) {
    return (an){
        .tag = AN__PRO,
        .val.an_pro = val,
    };
}

static an an_new_sum(an_sum val) {
    return (an){
        .tag = AN__SUM,
        .val.an_sum = val,
    };
}

static an an_new_cmp(an_cmp val) {
    return (an){
        .tag = AN__CMP,
        .val.an_cmp = val,
    };
}

static void an_drop(const an *self) {
    switch (self->tag) {
    case AN__VAR: {
        an_var_drop(&self->val.an_var);
        break;
    }
    case AN__VAL: {
        an_val_drop(&self->val.an_val);
        break;
    }
    case AN__LOG: {
        an_log_drop(&self->val.an_log);
        break;
    }
    case AN__PRO: {
        an_pro_drop(&self->val.an_pro);
        break;
    }
    case AN__SUM: {
        an_sum_drop(&self->val.an_sum);
        break;
    }
    case AN__CMP: {
        an_cmp_drop(&self->val.an_cmp);
        break;
    }
    default:
        break;
    }
}

static bool an_is_expr(const an *self) {
    switch (self->tag) {
    case AN__VAL:
    case AN__LOG:
    case AN__PRO:
    case AN__SUM:
    case AN__CMP: {
        return true;
    }
    default:
        return false;
    }
}

static void _an_print(const an *self, ast_printer *printer) {
    switch (self->tag) {
    case AN__VAR: {
        an_var_print(&self->val.an_var, printer);
        break;
    }
    case AN__VAL: {
        an_val_print(&self->val.an_val, printer);
        break;
    }
    case AN__LOG: {
        an_log_print(&self->val.an_log, printer);
        break;
    }
    case AN__PRO: {
        an_pro_print(&self->val.an_pro, printer);
        break;
    }
    case AN__SUM: {
        an_sum_print(&self->val.an_sum, printer);
        break;
    }
    case AN__CMP: {
        an_cmp_print(&self->val.an_cmp, printer);
        break;
    }
    default:
        UNREACHABLE;
    }
}

static void an_print(const an *self, bool tree) {
    ast_printer printer = ast_printer_new(stdout, tree);
    _an_print(self, &printer);
}
