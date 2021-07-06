#include "test/test.h"

int main() {
    TEST_RUN(entry);
    TEST_RUN(lmap);
    TEST_RUN(node);
    TEST_RUN(vec);

    puts("Ok");

    return 0;
}
