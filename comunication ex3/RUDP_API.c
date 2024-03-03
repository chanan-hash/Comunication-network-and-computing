#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include "RUDP_API.h"
/*
// A struct that represents RUDP Socket
typedef struct _rudp_socket
{
int socket_fd; // UDP socket file descriptor
bool isServer; // True if the RUDP socket acts like a server, false for client.
bool isConnected; // True if there is an active connection, false otherwise.
struct sockaddr_in dest_addr; // Destination address. Client fills it when it connects via rudp_connect(), server fills it when it accepts a connection via rudp_accept().
} RUDP_Socket;

// Allocates a new structure for the RUDP socket (contains basic information about the socket itself). Also creates a UDP socket as a baseline for the RUDP. isServer means that this socket acts like a server. If set to server socket, it also binds the socket to a specific port.
RUDP_Socket* rudp_socket(bool isServer, unsigned short int listen_port);

// Tries to connect to the other side via RUDP to given IP and port. Returns 0 on failure and 1 on success. Fails if called when the socket is connected/set to server.
int rudp_connect(RUDP_Socket *sockfd, const char *dest_ip, unsigned short int dest_port);

// Accepts incoming connection request and completes the handshake, returns 0 on failure and 1 on success. Fails if called when the socket is connected/set to client.
int rudp_accept(RUDP_Socket *sockfd);

// Receives data from the other side and put it into the buffer. Returns the number of received bytes on success, 0 if got FIN packet (disconnect), and -1 on error. Fails if called when the socket is disconnected.
int rudp_recv(RUDP_Socket *sockfd, void *buffer, unsigned int buffer_size);

// Sends data stores in buffer to the other side. Returns the number of sent bytes on success, 0 if got FIN packet (disconnect), and -1 on error. Fails if called when the socket is disconnected.
int rudp_send(RUDP_Socket *sockfd, void *buffer, unsigned int buffer_size);

// Disconnects from an actively connected socket. Returns 1 on success, 0 when the socket is already disconnected (failure).
int rudp_disconnect(RUDP_Socket *sockfd);

// This function releases all the memory allocation and resources of the socket.
int rudp_close(RUDP_Socket *sockfd);

*/
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
