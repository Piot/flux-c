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

static void tyran_memory_init_ex(tyran_memory* self, u8t* memory, size_t size, const char* debug)
{
	self->memory = memory;
	self->next = self->memory;
	self->size = size;
	self->debug = debug;
}

void tyran_memory_init(tyran_memory* self, size_t size, const char* debug)
{
	u8t* memory = tc_malloc(size);
	tyran_memory_init_ex(self, memory, size, debug);
}

void tyran_memory_clear(struct tyran_memory* self)
{
	self->next = self->memory;
}

static void size_string(char* buf, int size, int max_size)
{
	const int KILOBYTE = 1024;
	const int MEGABYTE = 1024 * KILOBYTE;
	int factor;
	char* suffix;

	if (size >= MEGABYTE) {
		suffix = "M";
		factor = MEGABYTE;
	} else if (size >= KILOBYTE) {
		suffix = "K";
		factor = KILOBYTE;
	} else {
		suffix = "b";
		factor = 1;
	}

	int value = size / factor;
	int percentage = 100;
	if (max_size != 0) {
		percentage = 100 * size / max_size;
	}
	sprintf(buf, "%d %s (%d %%)", value, suffix, percentage);
}

void tyran_memory_print_debug(const tyran_memory* self)
{
	size_t allocated = self->next - self->memory;
	char buf[32];
	size_string(buf, allocated, self->size);
	CLOG_INFO("mem %s %s", self->debug, buf);
}

static void* tyran_memory_alloc(tyran_memory* self, size_t size)
{
	if (self == 0) {
		CLOG_ERROR("NULL memory %lu  '%s'", size, self->debug);
		return 0;
	}
	if (size == 0) {
		return 0;
	}

	if (self->memory == 0) {
		CLOG_ERROR("Null memory!");
		return 0;
	}
	size_t allocated = self->next - self->memory;
	if (allocated + size > self->size) {
		CLOG_ERROR("Error: Out of memory! %s %zu %zu (%zu/%zu)", self->debug, size, allocated / size, allocated, self->size);
		return 0;
	}
	u8t* next = self->next;
	self->next += size;
	allocated += size;

	// TYRAN_LOG("alloc '%s' %d (%d / %d)", self->debug, size, allocated,
	// self->size);

	return next;
}

static void* tyran_memory_calloc(tyran_memory* self, size_t size)
{
	void* p = tyran_memory_alloc(self, size);
	if (p == 0) {
		return p;
	}
	tc_mem_clear(p, size);
	return p;
}

void tyran_memory_construct(struct tyran_memory* self, struct tyran_memory* parent, size_t size, const char* debug)
{
	void* memory = tyran_memory_calloc(parent, size);
	CLOG_INFO("Prepare memory %zu '%s/%s'", size, parent->debug, debug);
	tyran_memory_init_ex(self, memory, size, debug);
}

void* tyran_memory_alloc_debug(tyran_memory* self, size_t size, const char* source_file, int line, const char* description)
{
	if (self == 0) {
		CLOG_ERROR("NULL memory %zu %s:%d '%s'", size, source_file, line, description);
		return 0;
	}
	if (self->memory == 0) {
		CLOG_ERROR("Memory is null");
		return 0;
	}
	void* p = tyran_memory_alloc(self, size);
	if (size > 0 && p == 0) {
		CLOG_WARN("Out of memory %zu %s:%d '%s'", size, source_file, line, description);
	}
	return p;
}

void* tyran_memory_calloc_debug(tyran_memory* self, size_t size, const char* source_file, int line, const char* description)
{
	if (self == 0) {
		CLOG_ERROR("NULL memory %zu %s:%d '%s'", size, source_file, line, description);
		return 0;
	}

	// TYRAN_LOG("Allocating zeroed memory from '%s':%d", source_file, line);
	void* p = tyran_memory_calloc(self, size);
	if (size > 0 && p == 0) {
		CLOG_ERROR("Out of memory %zu %s:%d '%s'", size, source_file, line, description);
	}
	return p;
}

void tyran_memory_free(tyran_memory* memory, void* p)
{
	// tc_free(p);
}
