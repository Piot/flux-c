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
#include <flux/flux_socket.h>
#include <stdio.h>
#include <clog/clog.h>

#include <unistd.h>

clog_config g_clog;

static void console_log(enum clog_type type, const char* string)
{
	(void) type;
	puts(string);
}

int main(int argc, char* argv[])
{
	(void) argc;
	(void) argv;

	g_clog.log = console_log;
	CLOG_VERBOSE("example start")
	flux_socket socket;
	
	flux_init(&socket, "127.0.0.1", 32000);
	CLOG_VERBOSE("initialized")
	while (1) {
		CLOG_INFO("sending")
		flux_send(&socket, (const uint8_t*) "Hello", 5);
		sleep(1);
	}
}
