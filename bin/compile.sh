clang -g --std=c11 -Wall -Weverything -pedantic -Wno-unused-parameter -Wno-cast-align -D_POSIX_C_SOURCE=200112L -DCONFIGURATION_DEBUG ../src/lib/*.c   -I ../src/include/ -I ../deps/include  -shared
