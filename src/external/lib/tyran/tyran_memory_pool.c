/*

MIT License

Copyright (c) 2012 Peter Bjorklund

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
#include <tiny_libc/tiny_libc.h>
#include <clog/clog.h>
#include <tyran/tyran_memory.h>
#include <tyran/tyran_memory_pool.h>

static void tyran_memory_pool_initialize_entries(tyran_memory_pool* pool)
{
	tyran_memory_pool_entry* previous = 0;

	for (size_t i = 0; i < pool->max_count; ++i) {
		tyran_memory_pool_entry* e = &pool->entries[i];

		if (i == 0) {
			pool->first_free_index = 0;
		}
		e->allocated = TYRAN_FALSE;
		e->line = -1;
		e->file = 0;
		e->next_free_index = -1;
		e->pool = pool;

		if (previous) {
			previous->next_free_index = i;
		}
		previous = e;
	}

	pool->count = 0;
}

void tyran_memory_pool_clear(tyran_memory_pool* pool)
{
	tyran_memory_pool_initialize_entries(pool);
}

tyran_memory_pool* tyran_memory_pool_construct(tyran_memory* memory, size_t struct_size, size_t count, const char* type, const char* file, int line)
{
	CLOG_INFO("Memory alloc '%s' count:%zu", type, count);
	tyran_memory_pool* pool = TYRAN_MEMORY_ALLOC(memory, sizeof(tyran_memory_pool), "Memory pool");

	pool->entries = TYRAN_MEMORY_ALLOC(memory, (sizeof(tyran_memory_pool_entry)) * count, "Memory pool entries");
	pool->struct_size = struct_size;
	const int alignment = 16;
	int rest = struct_size % alignment;
	pool->aligned_struct_size = rest == 0 ? struct_size : struct_size + (alignment - rest);
	pool->memory = TYRAN_MEMORY_CALLOC(memory, (pool->aligned_struct_size) * count, "Memory pool entries");
	pool->type_string = type;
	pool->debug_source_file_line = line;
	pool->debug_source_file_name = file;

	pool->max_count = (int) count;
	tyran_memory_pool_initialize_entries(pool);
	return pool;
}

static void* tyran_memory_pool_alloc(tyran_memory_pool* pool)
{
	CLOG_ASSERT(pool->count < pool->max_count, "Out of memory in pool (%zu) %s %s:%d", pool->count, pool->type_string, pool->debug_source_file_name, pool->debug_source_file_line);

	if (pool->first_free_index == -1) {
		CLOG_INFO("First free is not available");
		return 0;
	}
	int found_index = pool->first_free_index;
	tyran_memory_pool_entry* e = &pool->entries[found_index];
	CLOG_ASSERT(e, "first free is null");
	pool->first_free_index = e->next_free_index;
	pool->count++;
	u8t* m = (u8t*) pool->memory + (found_index * pool->aligned_struct_size);
	e->allocated = TYRAN_TRUE;
	// CLOG_ASSERT(((tyran_pointer_to_number)p) % alignment == 0, "alignment error");
	return m;
}

void* tyran_memory_pool_alloc_debug(tyran_memory_pool* pool, const char* type_name, size_t struct_size)
{
	CLOG_ASSERT(tc_str_equal(pool->type_string, type_name), "Type name mismatch. Expected:%s received %s", pool->type_string, type_name);
	CLOG_ASSERT(pool->struct_size == struct_size, "Struct size mismatch. Expected %zu, received %zu", pool->struct_size, struct_size);
	return tyran_memory_pool_alloc(pool);
}

static void* tyran_memory_pool_calloc(tyran_memory_pool* pool)
{
	void* p = tyran_memory_pool_alloc(pool);

	CLOG_ASSERT(p, "not null");
	tc_mem_clear(p, pool->struct_size);
	return p;
}

void* tyran_memory_pool_calloc_debug(tyran_memory_pool* pool, const char* type_name, size_t struct_size)
{
	if (pool == 0) {
		CLOG_WARN("pool is null!!!");
	}
	CLOG_ASSERT(tc_strcmp(pool->type_string, type_name) == 0, "Type name mismatch. Expected:%s received %s", pool->type_string, type_name);
	CLOG_ASSERT(pool->struct_size == struct_size, "Struct size mismatch. Expected %zu, received %zu", pool->struct_size, struct_size);

	return tyran_memory_pool_calloc(pool);
}

void* tyran_memory_pool_pointer(tyran_memory_pool* pool, int index)
{
	u8t* m = (u8t*) pool->memory + (index * pool->aligned_struct_size);

	return m;
}

static int index_from_allocation(const tyran_memory_pool* pool, const void* p)
{
	int index_to_free = ((u8t*) p - pool->memory) / pool->aligned_struct_size;

	return index_to_free;
}

static tyran_memory_pool_entry* entry_from_allocation(const tyran_memory_pool* pool, const void* p, int* fetch_index)
{
	int index = index_from_allocation(pool, p);

	if (fetch_index != 0) {
		*fetch_index = index;
	}

	return &pool->entries[index];
}

tyran_boolean tyran_memory_pool_is_marked(const tyran_memory_pool* pool, const void* allocation)
{
	int index_to_free;
	const tyran_memory_pool_entry* e = entry_from_allocation(pool, allocation, &index_to_free);

	return e->marked_as_keep;
}

static void tyran_memory_pool_free_entry(tyran_memory_pool* pool, tyran_memory_pool_entry* e, int index_to_free)
{
	e->allocated = TYRAN_FALSE;
	pool->count--;

	// TYRAN_LOG("Freeing from memory pool '%s' (%zu) -> index: %d (count:%zu)", pool->type_string, pool->struct_size, index_to_free, pool->count);
	e->next_free_index = e->pool->first_free_index;
	pool->first_free_index = index_to_free;
}

void tyran_memory_pool_free(tyran_memory_pool* pool, void* p)
{
	int index_to_free;
	tyran_memory_pool_entry* e = entry_from_allocation(pool, p, &index_to_free);

	tyran_memory_pool_free_entry(pool, e, index_to_free);
}

char* tc_str_dup( const char* str)
{
	size_t size = tc_strlen(str);
	char* mem = tc_malloc_type_count(char, size + 1);

	tc_strncpy(mem, size + 1, str, size);
	mem[size] = 0;
	return mem;
}

void tyran_memory_pool_clear_marks(tyran_memory_pool* pool)
{
	for (size_t i = 0; i < pool->max_count; ++i) {
		tyran_memory_pool_entry* e = &pool->entries[i];
		e->marked_as_keep = TYRAN_FALSE;
	}
}

void tyran_memory_pool_mark(tyran_memory_pool* pool, void* allocation, int generation)
{
	int index_to_free;
	tyran_memory_pool_entry* e = entry_from_allocation(pool, allocation, &index_to_free);

	e->marked_as_keep = TYRAN_TRUE;
	e->generation = generation;
}

int tyran_memory_pool_sweep(tyran_memory_pool* pool, int max_generation)
{
	tyran_memory_pool_entry* entries = pool->entries;
	int freed = 0;

	for (size_t i = 0; i < pool->max_count; ++i) {
		tyran_memory_pool_entry* e = &entries[i];

		if (!e->marked_as_keep && e->generation <= max_generation) {
			tyran_memory_pool_free_entry(pool, e, i);
			freed++;
		}
	}

	return freed;
}

void tyran_memory_pool_print_debug(const tyran_memory_pool* pool)
{
	CLOG_INFO(" pool %s count:%zu max:%zu", pool->type_string, pool->count, pool->max_count);
}
