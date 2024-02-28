// A header file for RUDP APIvfunctions

#ifndef RUDP_API_H
#define RUDP_API_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

int rudp_socket_create();
int rudp_send(int socket, const void *data, size_t length);
int rudp_receive(int socket, void *buffer, size_t buffer_size);
void rudp_close(int socket);

#endif