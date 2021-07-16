#include "../test.h"

#include "../../src/tools/tok.h"

static void to_num_dec() {
    const char *ss[] = {
        "",
        "0",
        "1",
        "2",
        "12",
        "63",
        "64",
        "1000",
        "18446744073709551615",
    };
    u64 ns[] = {
        0,
        0,
        1,
        2,
        12,
        63,
        64,
        1000,
        18446744073709551615ULL,
    };

    for (word i = 0; i < ARRAY_SIZE(ss); ++i) {
        tok t = tok_new(TOK_DEC, SLICE_STR(ss[i]));
        u64 out = 0;
        ASSERT(tok_to_num(&t, &out));
        ASSERT(out == ns[i]);
    }

    tok t = tok_new(TOK_DEC, SLICE_STR("18446744073709551616"));
    u64 out = 0;
    ASSERT(!tok_to_num(&t, &out));
}

static void to_num_bin() {
    const char *ss[] = {
        "0b0",
        "0b1",
        "0b10",
        "0b1100",
        "0b111111",
        "0b1000000",
        "0b1111101000",
        "0b1111111111111111111111111111111111111111111111111111111111111111",
    };
    u64 ns[] = {
        0,
        1,
        2,
        12,
        63,
        64,
        1000,
        18446744073709551615ULL,
    };

    for (word i = 0; i < ARRAY_SIZE(ss); ++i) {
        tok t = tok_new(TOK_BIN, SLICE_STR(ss[i]));
        u64 out = 0;
        ASSERT(tok_to_num(&t, &out));
        ASSERT(out == ns[i]);
    }

    tok t = tok_new(
        TOK_BIN, SLICE_STR("0b10000000000000000000000000000000000000000000000000000000000000000"));
    u64 out = 0;
    ASSERT(!tok_to_num(&t, &out));
}

static void to_num_oct() {
    const char *ss[] = {
        "0o0",
        "0o1",
        "0o2",
        "0o14",
        "0o77",
        "0o100",
        "0o1750",
        "0o1777777777777777777777",
    };
    u64 ns[] = {
        0,
        1,
        2,
        12,
        63,
        64,
        1000,
        18446744073709551615ULL,
    };

    for (word i = 0; i < ARRAY_SIZE(ss); ++i) {
        tok t = tok_new(TOK_OCT, SLICE_STR(ss[i]));
        u64 out = 0;
        ASSERT(tok_to_num(&t, &out));
        ASSERT(out == ns[i]);
    }

    tok t = tok_new(TOK_OCT, SLICE_STR("0o2000000000000000000000"));
    u64 out = 0;
    ASSERT(!tok_to_num(&t, &out));
}

static void to_num_hex() {
    const char *ss[] = {
        "0x0",
        "0x1",
        "0x2",
        "0xC",
        "0x3F",
        "0x40",
        "0x3E8",
        "0xFFFFFFFFFFFFFFFF",
    };
    u64 ns[] = {
        0,
        1,
        2,
        12,
        63,
        64,
        1000,
        18446744073709551615ULL,
    };

    for (word i = 0; i < ARRAY_SIZE(ss); ++i) {
        tok t = tok_new(TOK_HEX, SLICE_STR(ss[i]));
        u64 out = 0;
        ASSERT(tok_to_num(&t, &out));
        ASSERT(out == ns[i]);
    }

    tok t = tok_new(TOK_HEX, SLICE_STR("0x10000000000000000"));
    u64 out = 0;
    ASSERT(!tok_to_num(&t, &out));
}

void test_tok() {
    to_num_dec();
    to_num_bin();
    to_num_oct();
    to_num_hex();
}
