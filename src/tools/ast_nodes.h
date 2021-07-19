#pragma once

#include "ast_types.h"

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

typedef enum {
    AN_PAT_TUPLE,
    AN_PAT_NAME,
    AN_PAT_UNDER,
} an_pat_tag;

typedef struct {
    an_pat_tag tag;
    union {
        struct {
            box name;
            an_tuple_pat tuple;
        } tuple;
        struct {
            bool tilda;
            box name;
        } name;
    } data;
} an_pat;

static an_pat an_pat_new_tuple(box name, an_tuple_pat tuple) {
    return (an_pat){
        .tag = AN_PAT_TUPLE,
        .data.tuple.name = name,
        .data.tuple.tuple = tuple,
    };
}

static an_pat an_pat_new_name(bool tilda, box name) {
    DEBUG_ASSERT(box_data(name));

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
        box_drop(self->data.tuple.name);
        an_tuple_pat_drop(&self->data.tuple.tuple);
        break;

    case AN_PAT_NAME:
        box_drop(self->data.name.name);
        break;

    default:
        break;
    }
}

typedef struct {
    an_pat pat;
    ast typ;
    ast val;
} an_decl;

static an_decl an_decl_new(an_pat pat, ast typ, ast val) {
    DEBUG_ASSERT(typ.tag == AST_NAME || typ.tag == AST_NONE);
    DEBUG_ASSERT(ast_tag_is_expr(val.tag) || val.tag == AST_NONE);

    return (an_decl){
        .pat = pat,
        .typ = typ,
        .val = val,
    };
}

static void an_decl_drop(an_decl *self) {
    an_pat_drop(&self->pat);
    ast_drop(&self->typ);
    ast_drop(&self->val);
}
