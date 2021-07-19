#pragma once

#include "../def.h"

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
            box tuple;
        } tuple;
        struct {
            bool tilda;
            box name;
        } name;
    } data;
} an_pat;

static an_pat an_pat_new_tuple(box name, box tuple) {
    DEBUG_ASSERT(box_data(tuple));
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
        box_drop(self->data.tuple.tuple);
        break;

    case AN_PAT_NAME:
        box_drop(self->data.name.name);
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

static void an_tuple_pat_drop(an_tuple_pat *self, hof on_item, word size) {
    box_drop_array(self->pats, on_item, size);
}

typedef struct {
    box pat;
    box typ;
    box val;
} an_decl;

static an_decl an_decl_new(box pat, box typ, box val) {
    DEBUG_ASSERT(box_data(pat));
    return (an_decl){
        .pat = pat,
        .typ = typ,
        .val = val,
    };
}

static void an_decl_drop(an_decl *self) {
    box_drop(self->pat);
    box_drop(self->typ);
    box_drop(self->val);
}
