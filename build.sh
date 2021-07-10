clang test/*.c -o run_test\
    -std=c11\
    -Wall -Wpedantic -Wextra -Wno-unused-function\
    -DDEBUG\
    && ./run_test
