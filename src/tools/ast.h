#pragma once

#include "../def.h"
#include "../tools/tok.h"

typedef enum {
    AST_NUM,
    AST_FLT,
    AST_STR,
    AST_CHR,
    AST_BOOL,
    AST_NAME,
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_REM,
    AST_ERR,
} ast_tag;

typedef struct {
    ast_tag tag;
    union {
        box x;
        u8 b1;
        u32 b4;
        u64 b8;
    } data;
} ast;

static ast ast_from_tok(const tok *t) {
    ast self;
    switch (t->tag) {
    case TOK_DEC:
    case TOK_BIN:
    case TOK_OCT:
    case TOK_HEX: {
        u64 out = 0;
        if (tok_to_num(t, &out)) {
            self.tag = AST_NUM;
            self.data.b8 = out;
            break;
        }

        self.tag = AST_ERR;
    }

    case TOK_FLT:
        self.tag = AST_FLT;
        self.data.x = box_from_slice(t->str);
        break;

    case TOK_STR:
        self.tag = AST_STR;
        self.data.x = box_from_slice(t->str);
        break;

    case TOK_CHR:
        self.tag = AST_CHR;
        u32 c = 0;
        slice_copy_to(t->str, &c);
        self.data.b4 = c;
        break;

    case TOK_BOOL:
        self.tag = AST_BOOL;
        self.data.b1 = *t->str.start == 't';
        break;

    case TOK_NAME:
        self.tag = AST_NAME;
        self.data.x = box_from_slice(t->str);
        break;

    default:
        UNREACHABLE;
    }

    return self;
}

static ast ast_from_pair(ast_tag tag, ast left, ast right) {
    DEBUG_ASSERT(
        tag == AST_ADD || tag == AST_SUB || tag == AST_MUL || tag == AST_DIV || tag == AST_REM);

    ast pair[] = {left, right};
    box x = BOX_FROM_ARRAY(pair);

    return (ast){
        .tag = tag,
        .data.x = x,
    };
}

static void ast_drop(const ast *self) {
    switch (self->tag) {
    case AST_FLT:
    case AST_STR:
    case AST_NAME:
        box_drop(self->data.x);
        break;

    case AST_ADD:
    case AST_SUB:
    case AST_MUL:
    case AST_DIV:
    case AST_REM: {
        box x = self->data.x;
        ast *pair = box_data(x);
        ast_drop(pair);
        ast_drop(pair + 1);
        box_drop(x);
        break;
    }

    default:
        break;
    }
}

typedef struct {
    FILE *file;
    bool pretty;
    bool first;
    word level;
} _ast_print_info;

static void _ast_print(const ast *self, _ast_print_info *info) {
    if (info->pretty) {
        if (info->first) {
            info->first = false;
        } else {
            fputc('\n', info->file);
        }

        for (word i = 0; i < info->level; ++i) {
            fprintf(info->file, "%s", "  ");
        }
        fprintf(info->file, "%s", COL_CYAN "- " COL_DEF);
    }

    const char *op;
    switch (self->tag) {
    case AST_NUM:
        fprintf(info->file, "%lu", self->data.b8);
        break;

    case AST_FLT:
    case AST_STR:
    case AST_NAME: {
        slice str = box_to_slice(self->data.x);
        slice_print(str, info->file);
        break;
    }

    case AST_CHR: {
        u32 c = self->data.b4;
        putchar(c);
        break;
    }

    case AST_BOOL:
        fprintf(info->file, "%s", self->data.b1 ? "true" : "false");
        break;

    case AST_ADD:
        op = "+";
        goto PRINT_OP;

    case AST_SUB:
        op = "-";
        goto PRINT_OP;

    case AST_MUL:
        op = "*";
        goto PRINT_OP;

    case AST_DIV:
        op = "/";
        goto PRINT_OP;

    case AST_REM:
        op = "%";

    PRINT_OP : {
        box x = self->data.x;
        ast *pair = box_data(x);
        if (info->pretty) {
            fprintf(info->file, "%s", op);
            ++info->level;
            _ast_print(pair, info);
            _ast_print(pair + 1, info);
            --info->level;
        } else {
            fprintf(info->file, "%s", COL_CYAN "[ " COL_DEF);
            _ast_print(pair, info);
            fprintf(info->file, " %s ", op);
            _ast_print(pair + 1, info);
            fprintf(info->file, "%s", COL_CYAN " ]" COL_DEF);
        }
        break;
    }

    case AST_ERR:
        fprintf(info->file, "%s", "ERR");
        break;

    default:
        UNREACHABLE;
    }
}

static void ast_print(const ast *self, FILE *file, bool pretty) {
    _ast_print_info info = {
        .file = file,
        .pretty = pretty,
        .first = true,
        .level = 0,
    };
    _ast_print(self, &info);
}
