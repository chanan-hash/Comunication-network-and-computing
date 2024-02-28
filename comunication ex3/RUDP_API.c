#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include "RUDP_API.h"

int rudp_socket_create()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0); // Creating the socket in UDP format

    if (sock == -1)
    {
        perror("socket(2)");
        return 1;
    }
    return sock;
}

// To send data we need the socket, a data to sent, length of the data, and struct of the address
int rudp_send(int socket, const void *data, size_t length, struct sockaddr_in *addr)
{
    int bytes_sent = sendto(socket, data, length, 0, (struct sockaddr *)addr, sizeof(struct sockaddr_in));
    if (bytes_sent <= 0)
    {
        perror("sendto(2)");
        return -1;
    }
    return bytes_sent;
}

int rudp_receive(int socket, void *buffer, size_t buffer_size, struct sockaddr_in *recv_addr)
{
    socklen_t recv_server_len = sizeof(struct sockaddr_in);
    int bytes_recived = recvfrom(socket, buffer, buffer_size, 0, (struct sockaddr *)recv_addr, &recv_server_len);

    if (bytes_recived <= 0)
    {
        perror("recfrom(2)");
        return -1;
    }
    return bytes_recived;
}

void rudp_close(int socket)
{
    close(socket);
}
