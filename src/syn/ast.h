#pragma once

#include "../def.h"
#include "ast_printer.h"

typedef struct an an;
static bool an_is_expr(const an *);
static void an_drop(const an *);
static void _an_print_ast_unary(const an *, const char *, ast_printer *);
static void _an_print_ast_binary(const an **, const char *, ast_printer *);
static void _an_print_ast_list(slice, const char *, ast_printer *);
static void _an_print(const an *, ast_printer *);

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
        _an_print_ast_unary(box_data(self->lhs), "!", printer);
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
    _an_print_ast_binary(ans, op, printer);
}

typedef enum {
    AN__VAR,
    AN__VAL,
    AN__LOG,
} an_tag;

typedef struct an {
    an_tag tag;
    union {
        an_var an_var;
        an_val an_val;
        an_log an_log;
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
    default:
        break;
    }
}

static bool an_is_expr(const an *self) {
    switch (self->tag) {
    case AN__VAL:
    case AN__LOG: {
        return true;
    }
    default:
        return false;
    }
}

static void _an_print_ast_unary(const an *self, const char *op, ast_printer *printer) {
    switch (printer->mode) {
    case AST_PRINTER_MODE_TREE: {
        ast_printer_print_node(printer);
        ast_printer_print_string(printer, op);
        ast_printer_nl(printer);
        ++printer->level;
        _an_print(self, printer);
        --printer->level;
        break;
    }
    case AST_PRINTER_MODE_PLAIN: {
        ast_printer_print_string(printer, op);
        _an_print(self, printer);
        break;
    }
    default:
        UNREACHABLE;
    }
}

static void _an_print_ast_binary(const an **self, const char *op, ast_printer *printer) {
    switch (printer->mode) {
    case AST_PRINTER_MODE_TREE: {
        ast_printer_print_node(printer);
        ast_printer_print_string(printer, op);
        ast_printer_nl(printer);
        ++printer->level;
        _an_print(self[0], printer);
        _an_print(self[1], printer);
        --printer->level;
        break;
    }
    case AST_PRINTER_MODE_PLAIN: {
        _an_print(self[0], printer);
        ast_printer_print_char(printer, ' ');
        ast_printer_print_string(printer, op);
        ast_printer_print_char(printer, ' ');
        _an_print(self[1], printer);
        break;
    }
    default:
        UNREACHABLE;
    }
}

static void _an_print_ast_list(slice list, const char *sep, ast_printer *printer) {
    switch (printer->mode) {
    case AST_PRINTER_MODE_TREE: {
        ast_printer_print_node(printer);
        ast_printer_print_range(printer);
        ++printer->level;
        for (const an *self = slice_start(list); self != slice_end(list); ++self) {
            ast_printer_print_node(printer);
            _an_print(self, printer);
        }
        --printer->level;
        break;
    }
    case AST_PRINTER_MODE_PLAIN: {
        for (const an *self = slice_start(list); self != slice_end(list); ++self) {
            if (self != slice_start(list)) {
                ast_printer_print_string(printer, sep);
            }

            _an_print(self, printer);
        }
        break;
    }
    default:
        UNREACHABLE;
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
    default:
        UNREACHABLE;
    }
}

static void an_print(const an *self, bool tree) {
    ast_printer printer = {
        .file = stdout,
        .mode = tree ? AST_PRINTER_MODE_TREE : AST_PRINTER_MODE_PLAIN,
        .level = 0,
        .color = true,
    };
    _an_print(self, &printer);
}
