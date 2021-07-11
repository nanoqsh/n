#pragma once

#include <stdio.h>

#define TEST_RUN(t)                                                                                \
    {                                                                                              \
        puts("Run " #t " test..");                                                                 \
        test_##t();                                                                                \
    }

void test_entry();
void test_lmap();
void test_node();
void test_vec();
void test_lex();
