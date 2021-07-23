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
    word index;
    bool color;
} ast_printer;

static ast_printer ast_printer_new(FILE *file, bool tree) {
    return (ast_printer){
        .file = file,
        .mode = tree ? AST_PRINTER_MODE_TREE : AST_PRINTER_MODE_PLAIN,
        .level = 0,
        .color = true,
    };
}

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

static void ast_printer_print_un(ast_printer *self, const char *op, hof print) {
    self->index = 0;
    switch (self->mode) {
    case AST_PRINTER_MODE_TREE: {
        ast_printer_print_node(self);
        ast_printer_print_string(self, op);
        ast_printer_nl(self);
        ++self->level;
        hof_call(print, self);
        --self->level;
        break;
    }
    case AST_PRINTER_MODE_PLAIN: {
        ast_printer_print_string(self, op);
        hof_call(print, self);
        break;
    }
    default:
        UNREACHABLE;
    }
}

static void ast_printer_print_bin(ast_printer *self, const char *op, hof print) {
    switch (self->mode) {
    case AST_PRINTER_MODE_TREE: {
        ast_printer_print_node(self);
        ast_printer_print_string(self, op);
        ast_printer_nl(self);
        ++self->level;
        self->index = 0;
        hof_call(print, self);
        self->index = 1;
        hof_call(print, self);
        --self->level;
        break;
    }
    case AST_PRINTER_MODE_PLAIN: {
        self->index = 0;
        hof_call(print, self);
        ast_printer_print_char(self, ' ');
        ast_printer_print_string(self, op);
        ast_printer_print_char(self, ' ');
        self->index = 1;
        hof_call(print, self);
        break;
    }
    default:
        UNREACHABLE;
    }
}

static void ast_printer_print_list(ast_printer *self, const char *sep, hof print, word len) {
    switch (self->mode) {
    case AST_PRINTER_MODE_TREE: {
        ast_printer_print_node(self);
        ast_printer_print_range(self);
        ++self->level;
        for (self->index = 0; self->index < len; ++self->index) {
            ast_printer_print_node(self);
            hof_call(print, self);
        }
        --self->level;
        break;
    }
    case AST_PRINTER_MODE_PLAIN: {
        for (self->index = 0; self->index < len; ++self->index) {
            if (self->index != 0) {
                ast_printer_print_string(self, sep);
            }

            hof_call(print, self);
        }
        break;
    }
    default:
        UNREACHABLE;
    }
}
