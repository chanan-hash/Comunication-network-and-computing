#include <stdio.h> // Standard input/output library
#include <arpa/inet.h> // For the in_addr structure and the inet_pton function
#include <sys/socket.h> // For the socket function
#include <unistd.h> // For the close function
#include <string.h> // For the memset function

/*
 * @brief The TCP's server IP address to connect to.
 * @note The default IP address is 127.0.0.1 (localhost)
*/
#define SERVER_IP "127.0.0.1"

/*
 * @brief The TCP's server port to connect to.
 * @note The default port is 5060.
*/
#define SERVER_PORT 5060

/*
 * @brief The buffer size to store the received message.
 * @note The default buffer size is 1024.
*/
#define BUFFER_SIZE 1024

/*
 * @brief TCP Client main function.
 * @param None
 * @return 0 if the client runs successfully, 1 otherwise.
*/
int main(void) {
    // The variable to store the socket file descriptor.
    int sock = -1;

    // The variable to store the server's address.
    struct sockaddr_in server;

    // Create a message to send to the server.
    char *message = "Hello from client";

    // Create a buffer to store the received message.
    char buffer[BUFFER_SIZE] = {0};

    // Reset the server structure to zeros.
    memset(&server, 0, sizeof(server));

    // Try to create a TCP socket (IPv4, stream-based, default protocol).
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // If the socket creation failed, print an error message and return 1.
    if (sock == -1)
    {
        perror("socket(2)");
        return 1;
    }

    // Convert the server's address from text to binary form and store it in the server structure.
    // This should not fail if the address is valid (e.g. "127.0.0.1").
    if (inet_pton(AF_INET, SERVER_IP, &server.sin_addr) <= 0)
    {
        perror("inet_pton(3)");
        close(sock);
        return 1;
    }

    // Set the server's address family to AF_INET (IPv4).
    server.sin_family = AF_INET;

    // Set the server's port to the defined port. Note that the port must be in network byte order,
    // so we first convert it to network byte order using the htons function.
    server.sin_port = htons(SERVER_PORT);

    fprintf(stdout, "Connecting to %s:%d...\n", SERVER_IP, SERVER_PORT);

    // Try to connect to the server using the socket and the server structure.
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect(2)");
        close(sock);
        return 1;
    }

    fprintf(stdout, "Successfully connected to the server!\n"
                    "Sending message to the server: %s\n", message);

    // Try to send the message to the server using the socket.
    int bytes_sent = send(sock, message, strlen(message) + 1, 0);

    // If the message sending failed, print an error message and return 1.
    // If no data was sent, print an error message and return 1. Only occurs if the connection was closed.
    if (bytes_sent <= 0)
    {
        perror("send(2)");
        close(sock);
        return 1;
    }

    fprintf(stdout, "Sent %d bytes to the server!\n"
                    "Waiting for the server to respond...\n", bytes_sent);

    // Try to receive a message from the server using the socket and store it in the buffer.
    int bytes_received = recv(sock, buffer, sizeof(buffer), 0);

    // If the message receiving failed, print an error message and return 1.
    // If no data was received, print an error message and return 1. Only occurs if the connection was closed.
    if (bytes_received <= 0)
    {
        perror("recv(2)");
        close(sock);
        return 1;
    }

    // Ensure that the buffer is null-terminated, no matter what message was received.
    // This is important to avoid SEGFAULTs when printing the buffer.
    if (buffer[BUFFER_SIZE - 1] != '\0')
        buffer[BUFFER_SIZE- 1] = '\0';

    // Print the received message.
    fprintf(stdout, "Got %d bytes from the server, which says: %s\n", bytes_received, buffer);

    // Close the socket with the server.
    close(sock);

    fprintf(stdout, "Connection closed!\n");

    // Return 0 to indicate that the client ran successfully.
    return 0;
}