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
#ifndef tyran_memory_pool_h
#define tyran_memory_pool_h

#include <tyran/tyran_types.h>

struct tyran_memory_pool;
struct tyran_memory;

typedef struct tyran_memory_pool_entry {
	const char* file;
	int line;
	tyran_boolean allocated;
	int next_free_index;
	struct tyran_memory_pool* pool;

	int generation;
	tyran_boolean marked_as_keep;
} tyran_memory_pool_entry;

typedef struct tyran_memory_pool {
	u8t* memory;
	size_t struct_size;
	size_t aligned_struct_size;
	size_t count;
	size_t max_count;
	tyran_memory_pool_entry* entries;
	const char* type_string;
	int first_free_index;
	const char* debug_source_file_name;
	int debug_source_file_line;
} tyran_memory_pool;

tyran_memory_pool* tyran_memory_pool_construct(struct tyran_memory* memory, size_t struct_size, size_t count, const char* type, const char* file, int line);
void* tyran_memory_pool_alloc_debug(tyran_memory_pool* pool, const char* type_name, size_t struct_size);
void* tyran_memory_pool_calloc_debug(tyran_memory_pool* pool, const char* type_name, size_t struct_size);
void* tyran_memory_pool_pointer(tyran_memory_pool* pool, int index);
void tyran_memory_pool_free(tyran_memory_pool* pool, void* allocation);
void tyran_memory_pool_clear(tyran_memory_pool* pool);
void tyran_memory_pool_print_debug(const tyran_memory_pool* pool);
void tyran_memory_pool_clear_marks(tyran_memory_pool* pool);
tyran_boolean tyran_memory_pool_is_marked(const tyran_memory_pool* pool, const void* allocation);
void tyran_memory_pool_mark(tyran_memory_pool* pool, void* allocation, int generation);
int tyran_memory_pool_sweep(tyran_memory_pool* pool, int max_generation);

#define TYRAN_MEMORY_POOL_CONSTRUCT(memory, T, count) tyran_memory_pool_construct(memory, sizeof(T), count, #T, __FILE__, __LINE__)
#define TYRAN_MEMORY_POOL_RAW(pool, index, T) (T*) tyran_memory_pool_pointer(pool, index)
#define TYRAN_CALLOC_TYPE(pool, T) (T*) tyran_memory_pool_calloc_debug(pool, #T, sizeof(T));
#define TYRAN_ALLOC_TYPE(pool, type) (type*) tyran_memory_pool_alloc_debug(pool, #type, sizeof(type));
#define TYRAN_ALLOC_FREE(pool, p) tyran_memory_pool_free(pool, p);
#define TYRAN_MEMORY_POOL_CLEAR(pool) tyran_memory_pool_clear(pool);

#endif
