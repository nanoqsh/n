#pragma once

#include "ast_types.h"

typedef enum {
    AN_PAT_TUPLE,
    AN_PAT_NAME,
    AN_PAT_UNDER,
} an_pat_tag;

typedef struct {
    an_pat_tag tag;
    union {
        struct {
            ast name;
            ast tuple_pat;
        } tuple;
        struct {
            bool tilda;
            ast name;
        } name;
    } data;
} an_pat;

static an_pat an_pat_new_tuple(ast name, ast tuple_pat) {
    DEBUG_ASSERT(name.tag == AST_NAME || name.tag == AST_NONE);
    DEBUG_ASSERT(tuple_pat.tag == AST_TUPLE_PAT);

    return (an_pat){
        .tag = AN_PAT_TUPLE,
        .data.tuple.name = name,
        .data.tuple.tuple_pat = tuple_pat,
    };
}

static an_pat an_pat_new_name(bool tilda, ast name) {
    DEBUG_ASSERT(name.tag == AST_NAME);

    return (an_pat){
        .tag = AN_PAT_NAME,
        .data.name.tilda = tilda,
        .data.name.name = name,
    };
}

static an_pat an_pat_new_under() {
    return (an_pat){
        .tag = AN_PAT_UNDER,
    };
}

static void an_pat_drop(an_pat *self) {
    switch (self->tag) {
    case AN_PAT_TUPLE:
        ast_drop(&self->data.tuple.name);
        ast_drop(&self->data.tuple.tuple_pat);
        break;

    case AN_PAT_NAME:
        ast_drop(&self->data.name.name);
        break;

    default:
        break;
    }
}

typedef struct {
    bool tilda;
    box pats;
} an_tuple_pat;

static an_tuple_pat an_tuple_pat_new(bool tilda, box pats) {
    return (an_tuple_pat){
        .tilda = tilda,
        .pats = pats,
    };
}

static void an_tuple_pat_drop(an_tuple_pat *self) {
    box_drop_array(self->pats, HOF(ast_drop), sizeof(ast));
}

typedef struct {
    ast pat;
    ast typ;
    ast val;
} an_decl;

static an_decl an_decl_new(ast pat, ast typ, ast val) {
    DEBUG_ASSERT(pat.tag == AST_PAT);
    DEBUG_ASSERT(typ.tag == AST_NONE);
    DEBUG_ASSERT(val.tag == AST_NONE);

    return (an_decl){
        .pat = pat,
        .typ = typ,
        .val = val,
    };
}

static void an_decl_drop(an_decl *self) {
    ast_drop(&self->pat);
    ast_drop(&self->typ);
    ast_drop(&self->val);
}
