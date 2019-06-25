clang -Wall -Weverything -pedantic -Wno-unused-parameter -Wno-cast-align ../src/lib/*.c ../deps/tiny-libc/src/lib/*.c  -I ../src/include/ -I ../deps/include -shared
