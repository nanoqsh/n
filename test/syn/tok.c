#include "../test.h"

#include "../../src/syn/tok.h"

static void to_num_dec() {
    const char *ss[] = {
        "_",
        "0_",
        "1_",
        "2_",
        "12_",
        "63_",
        "6_4",
        "100_0",
        "1844674407370955161_5",
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

    tok t = tok_new(TOK_DEC, SLICE_STR("18446744073709551616_"));
    u64 out = 0;
    ASSERT(!tok_to_num(&t, &out));
}

static void to_num_bin() {
    const char *ss[] = {
        "0b0_",
        "0b_1",
        "0b_10",
        "0b11_00",
        "0b1111_11",
        "0b1_00_00_00",
        "0b111110100_0",
        "0b1111111111111111111111111111111111111111111111111111111111111111_",
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
        TOK_BIN, SLICE_STR("0b10000000000000000000000000000000000000000000000000000000000000000_"));
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
        "0x0_",
        "0x1_",
        "0x2_",
        "0xC_",
        "0x3_F",
        "0x4_0",
        "0x3_E8",
        "0xF_____FFFFFFFFFFFFFFF",
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

    tok t = tok_new(TOK_HEX, SLICE_STR("0x10000000000000000_"));
    u64 out = 0;
    ASSERT(!tok_to_num(&t, &out));
}

void test_tok() {
    to_num_dec();
    to_num_bin();
    to_num_oct();
    to_num_hex();
}
