clang src/main.c src/test/*.c -o main -std=c11\
    -Wall -Wpedantic -Wextra -Wno-unused-function\
    -DDEBUG\
    && ./main
