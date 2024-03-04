#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // For the memset function
#include <arpa/inet.h>  // For the in_addr structure and the inet_pton function
#include <sys/socket.h> // For the socket function
#include <unistd.h>     // For the close function
#include <sys/time.h>   // For tv struct
#include "RUDP_API.h"

#define PORT 5061 // may be changed, temporary port for now

#define BUFFER_SIZE 1024 // need to be changed to 2mb

/***Will act as an server***/

int main(int argc, char *argv[])
{

    int reciver_socket = rudp_socket_create(); // creating the socket

    // Adding perror if not working from the function ****************************8

    // Accessing the addresses of the server and client
    struct sockaddr_in server;
    struct sockaddr_in client;

    socklen_t client_len = sizeof(client);

    char *message;

    int pot = 1;

    // Reseting the server and client structures to zeros
    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));

    if (reciver_socket == 1)
    { // 1 beacuse of the function creat_socket
        perror("socket(2)");
        return 1;
    }

    rudp_close(reciver_socket);
    return 0;
}