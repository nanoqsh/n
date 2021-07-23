#pragma once

#include "../def.h"
#include "ast_printer.h"
#include "ast_types.h"

typedef struct an_path {
    box head;
    box tail;
} an_path;

// head: str
// tail: [str]
static an_path an_path_new(box head, box tail) {
    return (an_path){
        .head = head,
        .tail = tail,
    };
}

static void _an_path_drop(const box *name) { box_drop(*name); }

static void an_path_drop(const an_path *self) {
    box_drop(self->head);
    box_drop_array(self->tail, HOF(_an_path_drop), sizeof(box));
}

static void an_path_print(const an_path *self, ast_printer *printer) {
    ast_printer_print_node(printer);
    slice tail = box_to_slice(self->tail);
    for (const box *name = slice_start(tail); name != slice_end(tail); ++name) {
        ast_printer_print_slice(printer, box_to_slice(*name));
        ast_printer_print_char(printer, '.');
    }
    ast_printer_print_slice(printer, box_to_slice(self->head));
}

typedef struct an_var {
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
    AN_VAL__PATH,
} an_val_tag;

typedef struct an_val {
    an_val_tag tag;
    union {
        an_path an_path;
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

static an_val an_val_new_path(an_path val) {
    return (an_val){
        .tag = AN_VAL__PATH,
        .val.an_path = val,
    };
}

static void an_val_drop(const an_val *self) {
    switch (self->tag) {
    case AN_VAL__FLT:
    case AN_VAL__STR: {
        box_drop(self->val.box);
        break;
    }
    case AN_VAL__PATH: {
        an_path_drop(&self->val.an_path);
        break;
    }
    default:
        break;
    }
}

static void an_val_print(const an_val *self, ast_printer *printer) {
    switch (self->tag) {
    case AN_VAL__INT: {
        ast_printer_print_node(printer);
        ast_printer_print_u64(printer, self->val.u64);
        break;
    }
    case AN_VAL__FLT: {
        ast_printer_print_node(printer);
        ast_printer_print_slice(printer, box_to_slice(self->val.box));
        break;
    }
    case AN_VAL__STR: {
        ast_printer_print_node(printer);
        ast_printer_print_string(printer, "\"");
        ast_printer_print_slice(printer, box_to_slice(self->val.box));
        ast_printer_print_string(printer, "\"");
        break;
    }
    case AN_VAL__CHR: {
        ast_printer_print_node(printer);
        ast_printer_print_string(printer, "'");
        ast_printer_print_char(printer, self->val.u32);
        ast_printer_print_string(printer, "'");
        break;
    }
    case AN_VAL__BOOL: {
        ast_printer_print_node(printer);
        ast_printer_print_string(printer, self->val.u8 ? "tru" : "fal");
        break;
    }
    case AN_VAL__PATH: {
        an_path_print(&self->val.an_path, printer);
        break;
    }
    default:
        UNREACHABLE;
    }
    ast_printer_nl(printer);
}

typedef enum {
    AN_EXPR__VAL,
    AN_EXPR__NEG,
    AN_EXPR__ADD,
    AN_EXPR__SUB,
    AN_EXPR__MUL,
    AN_EXPR__DIV,
    AN_EXPR__REM,
    AN_EXPR__NOT,
    AN_EXPR__AND,
    AN_EXPR__OR,
    AN_EXPR__XOR,
    AN_EXPR__EQ,
    AN_EXPR__NE,
    AN_EXPR__LT,
    AN_EXPR__GT,
    AN_EXPR__LE,
    AN_EXPR__GE,
    AN_EXPR__IF,
    AN_EXPR__FN,
    AN_EXPR__FN_CALL,
    AN_EXPR__BLOCK_EXPR,
} an_expr_tag;

typedef struct an_expr {
    an_expr_tag tag;
    union {
        an_val an_val;
        box box;
        box pair[2];
    } val;
} an_expr;

static an_expr an_expr_new_val(an_val val) {
    return (an_expr){
        .tag = AN_EXPR__VAL,
        .val.an_val = val,
    };
}

// val: an_expr
static an_expr an_expr_new_neg(box val) {
    return (an_expr){
        .tag = AN_EXPR__NEG,
        .val.box = val,
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_add(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__ADD,
        .val.pair = {lhs, rhs},
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_sub(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__SUB,
        .val.pair = {lhs, rhs},
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_mul(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__MUL,
        .val.pair = {lhs, rhs},
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_div(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__DIV,
        .val.pair = {lhs, rhs},
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_rem(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__REM,
        .val.pair = {lhs, rhs},
    };
}

// val: an_expr
static an_expr an_expr_new_not(box val) {
    return (an_expr){
        .tag = AN_EXPR__NOT,
        .val.box = val,
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_and(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__AND,
        .val.pair = {lhs, rhs},
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_or(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__OR,
        .val.pair = {lhs, rhs},
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_xor(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__XOR,
        .val.pair = {lhs, rhs},
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_eq(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__EQ,
        .val.pair = {lhs, rhs},
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_ne(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__NE,
        .val.pair = {lhs, rhs},
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_lt(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__LT,
        .val.pair = {lhs, rhs},
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_gt(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__GT,
        .val.pair = {lhs, rhs},
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_le(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__LE,
        .val.pair = {lhs, rhs},
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_ge(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__GE,
        .val.pair = {lhs, rhs},
    };
}

// lhs: an_expr
// rhs: an_expr
static an_expr an_expr_new_if(box lhs, box rhs) {
    return (an_expr){
        .tag = AN_EXPR__IF,
        .val.pair = {lhs, rhs},
    };
}

static void an_expr_drop(const an_expr *self) {
    switch (self->tag) {
    case AN_EXPR__VAL: {
        an_val_drop(&self->val.an_val);
        break;
    }
    case AN_EXPR__NEG:
    case AN_EXPR__NOT: {
        an_expr_drop(box_data(self->val.box));
        box_drop(self->val.box);
        break;
    }
    case AN_EXPR__ADD:
    case AN_EXPR__SUB:
    case AN_EXPR__MUL:
    case AN_EXPR__DIV:
    case AN_EXPR__REM:
    case AN_EXPR__AND:
    case AN_EXPR__OR:
    case AN_EXPR__XOR:
    case AN_EXPR__EQ:
    case AN_EXPR__NE:
    case AN_EXPR__LT:
    case AN_EXPR__GT:
    case AN_EXPR__LE:
    case AN_EXPR__GE:
    case AN_EXPR__IF: {
        box x = self->val.pair[0];
        an_expr_drop(box_data(x));
        box_drop(x);
        x = self->val.pair[1];
        an_expr_drop(box_data(x));
        box_drop(x);
        break;
    }
    default:
        UNREACHABLE;
    }
}

static void _an_expr_print(const an_expr *, ast_printer *);
static void _an_expr_print_un(ast_printer *printer, const an_expr *ex) {
    _an_expr_print(ex, printer);
}

static void _an_expr_print_bin(ast_printer *printer, const an_expr **ex) {
    _an_expr_print(ex[printer->index], printer);
}

static void _an_expr_print(const an_expr *self, ast_printer *printer) {
    const char *op;
    switch (self->tag) {
    case AN_EXPR__VAL: {
        an_val_print(&self->val.an_val, printer);
        break;
    }
    case AN_EXPR__NEG: {
        op = "-";
        goto PRINT_UN;
    }
    case AN_EXPR__NOT: {
        op = "!";
        goto PRINT_UN;
    }
    PRINT_UN : {
        const an_expr *ex = box_data(self->val.box);
        ast_printer_print_un(printer, op, HOF_WITH(_an_expr_print_un, ex));
        break;
    }
    case AN_EXPR__ADD: {
        op = "+";
        goto PRINT_BIN;
    }
    case AN_EXPR__SUB: {
        op = "-";
        goto PRINT_BIN;
    }
    case AN_EXPR__MUL: {
        op = "*";
        goto PRINT_BIN;
    }
    case AN_EXPR__DIV: {
        op = "/";
        goto PRINT_BIN;
    }
    case AN_EXPR__REM: {
        op = "%";
        goto PRINT_BIN;
    }
    case AN_EXPR__AND: {
        op = "&";
        goto PRINT_BIN;
    }
    case AN_EXPR__OR: {
        op = "|";
        goto PRINT_BIN;
    }
    case AN_EXPR__XOR: {
        op = "^";
        goto PRINT_BIN;
    }
    case AN_EXPR__EQ: {
        op = "==";
        goto PRINT_BIN;
    }
    case AN_EXPR__NE: {
        op = "!=";
        goto PRINT_BIN;
    }
    case AN_EXPR__LT: {
        op = "<";
        goto PRINT_BIN;
    }
    case AN_EXPR__GT: {
        op = ">";
        goto PRINT_BIN;
    }
    case AN_EXPR__LE: {
        op = "<=";
        goto PRINT_BIN;
    }
    case AN_EXPR__GE: {
        op = ">=";
        goto PRINT_BIN;
    }
    case AN_EXPR__IF: {
        op = "if";
        goto PRINT_BIN;
    }
    PRINT_BIN : {
        const an_expr *ex[] = {box_data(self->val.pair[0]), box_data(self->val.pair[1])};
        ast_printer_print_bin(printer, op, HOF_WITH(_an_expr_print_bin, ex));
        break;
    }
    default:
        UNREACHABLE;
    }
}

static void an_expr_print(const an_expr *self, bool tree) {
    ast_printer printer = ast_printer_new(stdout, tree);
    _an_expr_print(self, &printer);
}
