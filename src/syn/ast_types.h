#pragma once

#define AN(a)                                                                                      \
    typedef struct an_##a an_##a;                                                                  \
    static void an_##a##_drop(const an_##a *);

AN(path)
AN(var)
AN(val)
AN(expr)

#undef AN
