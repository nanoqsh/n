#pragma once

#include "../def.h"
#include <stdlib.h>

typedef enum {
    AST_PRINTER_MODE_TREE,
    AST_PRINTER_MODE_PLAIN,
} ast_printer_mode;

typedef struct {
    FILE *file;
    ast_printer_mode mode;
    word level;
    bool color;
} ast_printer;

typedef struct an an;
static void _an_print(const an *, ast_printer *);

static void ast_printer_print_string(ast_printer *self, const char *text) {
    fprintf(self->file, "%s", text);
}

static void ast_printer_print_slice(ast_printer *self, slice text) {
    slice_print(text, self->file);
}

static void ast_printer_print_u64(ast_printer *self, u64 val) { fprintf(self->file, "%lu", val); }

static void ast_printer_print_char(ast_printer *self, u32 val) { fputc(val, self->file); }

static void ast_printer_nl(ast_printer *self) {
    if (self->mode != AST_PRINTER_MODE_TREE) {
        return;
    }

    fputc('\n', self->file);
}

static void _ast_printer_print_color_string(ast_printer *self, const char *text) {
    if (self->color) {
        fprintf(self->file, "%s", COL_CYAN);
    }
    fprintf(self->file, "%s", text);
    if (self->color) {
        fprintf(self->file, "%s", COL_DEF);
    }
}

static void ast_printer_print_node(ast_printer *self) {
    if (self->mode != AST_PRINTER_MODE_TREE) {
        return;
    }

    for (word i = 0; i < self->level; ++i) {
        fprintf(self->file, "%s", "  ");
    }

    _ast_printer_print_color_string(self, "- ");
}

static void ast_printer_print_range(ast_printer *self) {
    if (self->mode != AST_PRINTER_MODE_TREE) {
        return;
    }

    _ast_printer_print_color_string(self, " .. ");
}

static void ast_printer_print_ast_unary(ast_printer *self, const an *ast, const char *op) {
    switch (self->mode) {
    case AST_PRINTER_MODE_TREE: {
        ast_printer_print_node(self);
        ast_printer_print_string(self, op);
        ast_printer_nl(self);
        ++self->level;
        _an_print(ast, self);
        --self->level;
        break;
    }
    case AST_PRINTER_MODE_PLAIN: {
        ast_printer_print_string(self, op);
        _an_print(ast, self);
        break;
    }
    default:
        UNREACHABLE;
    }
}

static void ast_printer_print_ast_binary(ast_printer *self, const an **ast, const char *op) {
    switch (self->mode) {
    case AST_PRINTER_MODE_TREE: {
        ast_printer_print_node(self);
        ast_printer_print_string(self, op);
        ast_printer_nl(self);
        ++self->level;
        _an_print(ast[0], self);
        _an_print(ast[1], self);
        --self->level;
        break;
    }
    case AST_PRINTER_MODE_PLAIN: {
        _an_print(ast[0], self);
        ast_printer_print_char(self, ' ');
        ast_printer_print_string(self, op);
        ast_printer_print_char(self, ' ');
        _an_print(ast[1], self);
        break;
    }
    default:
        UNREACHABLE;
    }
}

static void ast_printer_print_ast_list(ast_printer *self, slice list, const char *sep, word size) {
    switch (self->mode) {
    case AST_PRINTER_MODE_TREE: {
        ast_printer_print_node(self);
        ast_printer_print_range(self);
        ++self->level;
        for (const u8 *item = slice_start(list); item != slice_end(list); item += size) {
            ast_printer_print_node(self);
            _an_print((const an *)item, self);
        }
        --self->level;
        break;
    }
    case AST_PRINTER_MODE_PLAIN: {
        for (const u8 *item = slice_start(list); item != slice_end(list); item += size) {
            if (item != slice_start(list)) {
                ast_printer_print_string(self, sep);
            }

            _an_print((const an *)item, self);
        }
        break;
    }
    default:
        UNREACHABLE;
    }
}
