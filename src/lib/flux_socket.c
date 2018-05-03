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
#include <arpa/inet.h>
#include <clog/clog.h>
#include <errno.h>
#include <fcntl.h>
#include <flux/flux_socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

static int create()
{
	int handle = socket(PF_INET, SOCK_DGRAM, 0);
	fcntl(handle, F_SETFL, O_NONBLOCK);
	return handle;
}

static void set_peer_address(flux_socket* self, const char* name, int port)
{
	struct sockaddr_in* peer = &self->peer_address;

	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; // AF_INET
	hints.ai_socktype = SOCK_DGRAM;

	struct addrinfo* result;

	int s = getaddrinfo(name, 0, &hints, &result);
	if (s < 0) {
		CLOG_WARN("set_peer_address Error!%d", s);
		return;
	}

	const struct addrinfo* first = result;
	memset(peer, 0, sizeof(*peer));
	struct sockaddr_in* in_addr = (struct sockaddr_in*) first->ai_addr;
	memcpy(peer, in_addr, first->ai_addrlen);
	peer->sin_port = htons(port);
	freeaddrinfo(result);
}

void flux_init(flux_socket* self, const char* name, int port)
{
	self->handle = create();
	set_peer_address(self, name, port);
}

tyran_boolean flux_send(flux_socket* self, const uint8_t* data, size_t size)
{
	ssize_t number_of_octets_sent = sendto(self->handle, data, size, 0, (struct sockaddr*) &self->peer_address, sizeof(self->peer_address));

	if (number_of_octets_sent < 0) {
		CLOG_WARN("Error send! %ld\n", number_of_octets_sent);
		return TYRAN_FALSE;
	}

	return ((size_t) number_of_octets_sent == size);
}

size_t flux_receive(flux_socket* self, uint8_t* data, size_t size)
{
	struct sockaddr_in from_who;

	socklen_t addr_size = sizeof(from_who);
	ssize_t number_of_octets = recvfrom(self->handle, data, size, 0, (struct sockaddr*) &from_who, &addr_size);
	if (number_of_octets == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return 0;
		} else {
			CLOG_WARN("Error receive! %ld\n", number_of_octets);
			return 0;
		}
	}

	return number_of_octets;
}
