/*

MIT License

Copyright (c) 2017 Peter Bjorklund

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#ifndef TYRAN_MEMORY_H
#define TYRAN_MEMORY_H

#include <tyran/tyran_types.h>

typedef struct tyran_memory {
	u8t* memory;
	size_t size;
	u8t* next;
	const char* debug;
} tyran_memory;

void* tyran_memory_alloc_debug(struct tyran_memory* memory, size_t size, const char* source_file, int line, const char* description);

void* tyran_memory_calloc_debug(struct tyran_memory* memory, size_t size, const char* source_file, int line, const char* description);

void tyran_memory_init(struct tyran_memory* memory, size_t size, const char* debug);
void tyran_memory_clear(struct tyran_memory* memory);
void tyran_memory_print_debug(const struct tyran_memory* memory);
void tyran_memory_construct(struct tyran_memory* memory, struct tyran_memory* parent, size_t size, const char* debug);

void tyran_memory_free(struct tyran_memory* memory, void* p);

#define TYRAN_MEMORY_ALLOC(memory, size, description) tyran_memory_alloc_debug(memory, size, __FILE__, __LINE__, description)
#define TYRAN_MEMORY_CALLOC(memory, size, description) tyran_memory_calloc_debug(memory, size, __FILE__, __LINE__, description)

#define TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, type, count) (type*) TYRAN_MEMORY_CALLOC(memory, count * sizeof(type), #type);
#define TYRAN_MEMORY_CALLOC_TYPE(memory, type) (type*) TYRAN_MEMORY_CALLOC(memory, sizeof(type), #type);
#define TYRAN_MEMORY_ALLOC_TYPE(memory, type) (type*) TYRAN_MEMORY_ALLOC(memory, sizeof(type), #type);
#define TYRAN_MEMORY_ALLOC_TYPE_COUNT(memory, type, count) (type*) TYRAN_MEMORY_ALLOC(memory, count * sizeof(type), #type);

#endif
