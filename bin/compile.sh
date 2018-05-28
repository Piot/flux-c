clang -Wall -Weverything -pedantic -Wno-unused-parameter -Wno-cast-align ../src/lib/*.c ../deps/tiny-libc/*.c  -I ../src/include/ -I ../deps -shared
