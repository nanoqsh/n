#pragma once

#include "../src/lib/col.h"
#include <stdio.h>

#define TEST_RUN(t)                                                                                \
    {                                                                                              \
        puts(COL_CYAN "Run " #t " test.." COL_DEF);                                                \
        test_##t();                                                                                \
    }

void test_entry();
void test_hof();
void test_lmap();
void test_node();
void test_vec();

void test_ast();
void test_lex();
void test_tok();
