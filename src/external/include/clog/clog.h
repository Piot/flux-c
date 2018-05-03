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
#ifndef clog_h
#define clog_h

#include <tiny_libc/tiny_libc.h>

#include <stdio.h>
#include <stdlib.h>

enum clog_type {
	CLOG_TYPE_VERBOSE,
	CLOG_TYPE_INFO,
	CLOG_TYPE_WARN,
	CLOG_TYPE_ERROR
};

typedef struct clog {
	void (*log)(enum clog_type type, const char* string);
} clog;

extern clog g_clog;

#define CLOG_EX(logtype, ...)                                                  \
	{                                                                          \
		char _temp_str[1024];                                                  \
		sprintf(_temp_str, __VA_ARGS__);                                       \
		g_clog.log(logtype, _temp_str);                                        \
	}

#if defined CONFIGURATION_DEBUG

#define CLOG_VERBOSE(...) CLOG_EX(CLOG_TYPE_VERBOSE, __VA_ARGS__)
#define CLOG_INFO(...) CLOG_EX(CLOG_TYPE_INFO, __VA_ARGS__)
#define CLOG_WARN(...) CLOG_EX(CLOG_TYPE_WARN, __VA_ARGS__)
#define CLOG_SOFT_ERROR(...) CLOG_WARN(__VA_ARGS__);
#define CLOG_BREAK
#define CLOG_ERROR(...)                                                        \
	CLOG_EX(LOG_TYPE_ERROR, __VA_ARGS__);                                      \
	CLOG_OUTPUT(__VA_ARGS__);                                                  \
	CLOG_BREAK;
#define CLOG_ASSERT(expression, ...)                                           \
	if (!(expression)) {                                                       \
		CLOG_ERROR(__VA_ARGS__);                                               \
		CLOG_BREAK;                                                            \
	}

#else

#define CLOG_VERBOSE(...)
#define CLOG_INFO(...)
#define CLOG_WARN(...)
#define CLOG_SOFT_ERROR(...) CLOG_EX(CLOG_TYPE_ERROR, __VA_ARGS__);
#define CLOG_BREAK abort()
#define CLOG_ERROR(...)                                                        \
	CLOG_EX(CLOG_TYPE_ERROR, __VA_ARGS__);                                     \
	CLOG_BREAK;

#define CLOG_ASSERT(expression, ...)

#endif

#define CLOG_OUTPUT(...)                                                       \
	{                                                                          \
		tc_fprintf(stdout, __VA_ARGS__);                                       \
		tc_fprintf(stdout, "\n");                                              \
		tc_fflush(stdout);                                                     \
	}

#endif
