#pragma once

#include "../def.h"
#include "../tools/tok.h"

typedef enum {
    // expr
    // leaf
    AST_NUM,
    AST_FLT,
    AST_STR,
    AST_CHR,
    AST_BOOL,
    AST_NAME,
    // end leaf
    AST_NEG,
    AST_NOT,
    // binary
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_REM,
    AST_EQ,
    AST_NEQ,
    AST_LT,
    AST_GT,
    AST_LE,
    AST_GE,
    AST_AND,
    AST_OR,
    AST_XOR,
    // end binary
    AST_TUPLE,
    AST_BLOCK,
    // end expr
    AST_ERR,
} ast_tag;

bool ast_tag_is_leaf(ast_tag self) { return self >= AST_NUM && self <= AST_NAME; }

bool ast_tag_is_expr(ast_tag self) { return self >= AST_NUM && self <= AST_BLOCK; }

bool ast_tag_is_binary(ast_tag self) { return self >= AST_ADD && self <= AST_XOR; }

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

static ast ast_single(ast_tag tag, ast node) {
    DEBUG_ASSERT(!ast_tag_is_binary(tag));
    DEBUG_ASSERT(ast_tag_is_expr(node.tag));

    return (ast){
        .tag = tag,
        .data.x = BOX(&node),
    };
}

static ast ast_binary(ast_tag tag, ast left, ast right) {
    DEBUG_ASSERT(ast_tag_is_binary(tag));
    DEBUG_ASSERT(ast_tag_is_expr(left.tag));
    DEBUG_ASSERT(ast_tag_is_expr(right.tag));

    ast pair[] = {left, right};
    return (ast){
        .tag = tag,
        .data.x = BOX_FROM_ARRAY(pair),
    };
}

static ast ast_list(ast_tag tag, slice list) {
    DEBUG_ASSERT(tag == AST_BLOCK || tag == AST_TUPLE);

    return (ast){
        .tag = tag,
        .data.x = box_from_slice(list),
    };
}

static void ast_drop(const ast *self) {
    switch (self->tag) {
    case AST_FLT:
    case AST_STR:
    case AST_NAME:
        box_drop(self->data.x);
        break;

    case AST_NEG:
    case AST_NOT: {
        box x = self->data.x;
        ast *node = box_data(x);
        ast_drop(node);
        box_drop(x);
        break;
    }

    case AST_ADD:
    case AST_SUB:
    case AST_MUL:
    case AST_DIV:
    case AST_REM:
    case AST_EQ:
    case AST_NEQ:
    case AST_LT:
    case AST_GT:
    case AST_LE:
    case AST_GE:
    case AST_AND:
    case AST_OR:
    case AST_XOR: {
        box x = self->data.x;
        ast *pair = box_data(x);
        ast_drop(pair);
        ast_drop(pair + 1);
        box_drop(x);
        break;
    }

    case AST_TUPLE:
    case AST_BLOCK: {
        box x = self->data.x;
        slice list = box_to_slice(x);
        for (const ast *item = slice_start(list); item != slice_end(list); ++item) {
            ast_drop(item);
        }

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

#define INDENT                                                                                     \
    for (word i = 0; i < info->level; ++i) {                                                       \
        fprintf(info->file, "%s", "  ");                                                           \
    }

#define INDENT2                                                                                    \
    INDENT;                                                                                        \
    INDENT;

static void _ast_print(const ast *self, _ast_print_info *info) {
    if (info->pretty) {
        if (info->first) {
            info->first = false;
        } else {
            fputc('\n', info->file);
        }

        INDENT;
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

    case AST_NEG:
        op = "-";
        goto PRINT_UN;

    case AST_NOT:
        op = "!";
        goto PRINT_UN;

    PRINT_UN : {
        box x = self->data.x;
        ast *node = box_data(x);
        if (info->pretty) {
            fprintf(info->file, "%s", op);
            ++info->level;
            _ast_print(node, info);
            --info->level;
        } else {
            fprintf(info->file, "%s", op);
            _ast_print(node, info);
        }
        break;
    }

    case AST_ADD:
        op = "+";
        goto PRINT_BIN;

    case AST_SUB:
        op = "-";
        goto PRINT_BIN;

    case AST_MUL:
        op = "*";
        goto PRINT_BIN;

    case AST_DIV:
        op = "/";
        goto PRINT_BIN;

    case AST_REM:
        op = "%";
        goto PRINT_BIN;

    case AST_EQ:
        op = "==";
        goto PRINT_BIN;

    case AST_NEQ:
        op = "!=";
        goto PRINT_BIN;

    case AST_LT:
        op = "<";
        goto PRINT_BIN;

    case AST_GT:
        op = ">";
        goto PRINT_BIN;

    case AST_LE:
        op = "<=";
        goto PRINT_BIN;

    case AST_GE:
        op = ">=";
        goto PRINT_BIN;

    case AST_AND:
        op = "&";
        goto PRINT_BIN;

    case AST_OR:
        op = "|";
        goto PRINT_BIN;

    case AST_XOR:
        op = "^";
        goto PRINT_BIN;

    PRINT_BIN : {
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

    case AST_TUPLE: {
        box x = self->data.x;
        slice list = box_to_slice(x);
        if (info->pretty) {
            fprintf(info->file, "%s", "[ " COL_CYAN ".." COL_DEF " ]");
            ++info->level;
            for (const ast *item = slice_start(list); item != slice_end(list); ++item) {
                _ast_print(item, info);
            }
            --info->level;
        } else {
            fprintf(info->file, "%s", "[ ");
            ++info->level;
            for (const ast *item = slice_start(list); item != slice_end(list); ++item) {
                if (item != slice_start(list)) {
                    fprintf(info->file, "%s", ", ");
                }

                _ast_print(item, info);
            }
            --info->level;
            fprintf(info->file, "%s", " ]");
        }
        break;
    }

    case AST_BLOCK: {
        box x = self->data.x;
        slice list = box_to_slice(x);
        if (info->pretty) {
            fprintf(info->file, "%s", "{ " COL_CYAN ".." COL_DEF " }");
            ++info->level;
            for (const ast *item = slice_start(list); item != slice_end(list); ++item) {
                _ast_print(item, info);
            }
            --info->level;
        } else {
            fprintf(info->file, "%s", "{\n");
            ++info->level;
            for (const ast *item = slice_start(list); item != slice_end(list); ++item) {
                INDENT2;
                _ast_print(item, info);
                fputc('\n', info->file);
            }
            --info->level;
            INDENT2;
            fprintf(info->file, "%s", "}");
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

#undef INDENT
#undef INDENT2
