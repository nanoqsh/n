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

static void ast_printer_print_string(ast_printer *self, const char *text) {
    fprintf(self->file, "%s", text);
}

static void ast_printer_print_slice(ast_printer *self, slice text) {
    slice_print(text, self->file);
}

static void ast_printer_print_u64(ast_printer *self, u64 val) { fprintf(self->file, "%lu", val); }

static void ast_printer_print_char(ast_printer *self, u32 val) { fputc(val, self->file); }

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
