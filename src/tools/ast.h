#pragma once

#include "../def.h"
#include "../tools/tok.h"
#include "ast_nodes.h"

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

        self.tag = AST_NONE;
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

    case TOK_TRU:
        self.tag = AST_BOOL;
        self.data.b1 = true;
        break;

    case TOK_FAL:
        self.tag = AST_BOOL;
        self.data.b1 = false;
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

static void ast_drop(const ast *self) {
    box x;
    switch (self->tag) {
    case AST_FLT:
    case AST_STR:
    case AST_NAME:
        x = self->data.x;
        break;

    case AST_NEG:
    case AST_NOT: {
        x = self->data.x;
        ast *node = box_data(x);
        ast_drop(node);
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
        x = self->data.x;
        ast *pair = box_data(x);
        ast_drop(pair);
        ast_drop(pair + 1);
        break;
    }

    case AST_TUPLE:
    case AST_BLOCK: {
        x = self->data.x;
        slice list = box_to_slice(x);
        for (const ast *item = slice_start(list); item != slice_end(list); ++item) {
            ast_drop(item);
        }
        break;
    }

    case AST_PAT: {
        x = self->data.x;
        an_pat *a = box_data(x);
        an_pat_drop(a);
        break;
    }

    case AST_TUPLE_PAT: {
        x = self->data.x;
        an_tuple_pat *a = box_data(x);
        an_tuple_pat_drop(a);
        break;
    }

    case AST_DECL: {
        x = self->data.x;
        an_decl *a = box_data(x);
        an_decl_drop(a);
        break;
    }

    default:
        return;
    }

    box_drop(x);
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

static void _ast_print(const ast *, _ast_print_info *);
static void _ast_print_slice(slice list, _ast_print_info *info, const char *sep, bool indent) {
    ++info->level;
    for (const ast *item = slice_start(list); item != slice_end(list); ++item) {
        if (indent) {
            INDENT2;
        }

        if (item != slice_start(list)) {
            fprintf(info->file, "%s", sep);
        }

        _ast_print(item, info);
    }
    --info->level;

    if (indent) {
        INDENT2;
    }
}

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
    case AST_NONE:
        UNREACHABLE;

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
        fprintf(info->file, "%s", self->data.b1 ? "tru" : "fal");
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
        fprintf(info->file, "%s", info->pretty ? "[ " COL_CYAN ".." COL_DEF " ]" : "[ ");
        _ast_print_slice(list, info, ", ", false);
        if (!info->pretty) {
            fprintf(info->file, "%s", " ]");
        }
        break;
    }

    case AST_BLOCK: {
        box x = self->data.x;
        slice list = box_to_slice(x);
        fprintf(info->file, "%s", info->pretty ? "{ " COL_CYAN ".." COL_DEF " }" : "{\n");
        _ast_print_slice(list, info, info->pretty ? "" : "\n    ", true);
        if (!info->pretty) {
            fprintf(info->file, "%s", "\n}");
        }
        break;
    }

    case AST_PAT: {
        box x = self->data.x;
        an_pat *a = box_data(x);
        switch (a->tag) {
        case AN_PAT_TUPLE: {
            ++info->level;

            ast name = a->data.tuple.name;
            if (name.tag != AST_NONE) {
                _ast_print(&name, info);
                fputc(' ', info->file);
            }

            ast tuple_pat = a->data.tuple.tuple_pat;
            _ast_print(&tuple_pat, info);

            --info->level;
            break;
        }

        case AN_PAT_NAME: {
            if (a->data.name.tilda) {
                fputc('~', info->file);
            }

            ast name = a->data.name.name;
            _ast_print(&name, info);
            break;
        }

        case AN_PAT_UNDER:
            fputc('_', info->file);
            break;

        default:
            UNREACHABLE;
        }

        break;
    }

    case AST_TUPLE_PAT: {
        box x = self->data.x;
        an_tuple_pat *a = box_data(x);
        if (a->tilda) {
            fputc('~', info->file);
        }

        slice list = box_to_slice(a->pats);
        fprintf(info->file, "%s", info->pretty ? "[ " COL_CYAN ".." COL_DEF " ]" : "[");
        _ast_print_slice(list, info, info->pretty ? "" : ", ", false);
        if (!info->pretty) {
            fputc(']', info->file);
        }
        break;
    }

    case AST_DECL: {
        box x = self->data.x;
        an_decl *a = box_data(x);

        fprintf(info->file, "%s", "let ");

        ++info->level;
        ast pat = a->pat;
        _ast_print(&pat, info);

        ast typ = a->typ;
        if (typ.tag != AST_NONE) {
            fprintf(info->file, "%s", ": ");
            _ast_print(&typ, info);
        }

        ast val = a->val;
        if (val.tag != AST_NONE) {
            fprintf(info->file, "%s", " = ");
            _ast_print(&val, info);
        }
        --info->level;

        break;
    }

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
