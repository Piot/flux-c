rm flux-test
clang -Wall -pedantic lib/*.c examples/*.c -I include/ -I external/include -o flux-test
