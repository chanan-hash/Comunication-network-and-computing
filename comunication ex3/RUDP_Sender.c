#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // For the memset function
#include <arpa/inet.h>  // For the in_addr structure and the inet_pton function
#include <sys/socket.h> // For the socket function
#include <unistd.h>     // For the close function
#include <sys/time.h>   // For tv struct
#include "RUDP_API.h"

#define PORT 5061 // may be changed, temporary port for now
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024 // need to be changed to 2mb

/***Will act as an client***/

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        // print something
        return 1;
    }

    char *reciver_ip = argv[0];
    int port = atoi(argv[1]); // argument to integer

    int sender_socket = rudp_socket_create();

    struct sockaddr_in reciver_address; // geeting the reciver address

    memset(&reciver_address, 0, sizeof(reciver_address));
    // reciver_address.sin_addr.s_addr = INADDR_ANY;
    reciver_address.sin_family = AF_INET;
    reciver_address.sin_port = htons(PORT); // Converts to Big or Little Endian

    if (inet_pton(AF_INET, SERVER_IP, &(reciver_address.sin_addr.s_addr)) <= 0)
    {
        perror("inet_pton(3)");
        rudp_close(sender_socket);
        return 1;
    }

    char buffer[BUFFER_SIZE] = {0};

    size_t bytes_read;

    // rudp_send(sender_socket, buffer, bytes_read,);

    rudp_close(sender_socket);
    return 0;
}