### flux-c

Datagram (UDP) communications in c99.

##### Usage

```c
flux_socket socket;

flux_init(&socket, "example.com", 32102);

flux_send(&socket, (const uint8_t*) "Hello", 5);

uint8_t data[512];
size_t octets_received = flux_receive(&socket, data, 512);
```
