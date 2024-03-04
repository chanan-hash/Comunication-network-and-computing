#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include "RUDP_API.h"

// define more functions
int checksum(RUDP *rudp); // to check if the whole packet came
int wait_for_acknowledgement(int socket, int sequal_num, clock_t s, clock_t t);
int send_acknowledgement(int socket, RUDP *prudp);
int set_time(int socket, int time);

int seq_num = 0;

// Adding the print function
void print_whole(RUDP *rudp)
{
    // Printing the flags data
    printf("RUDP packet:\n");
    printf("\tThe flags:");
    printf("\t\t SYN: %u\n", rudp->flags.SYN);
    printf("\t\t ACK: %u\n", rudp->flags.ACK);
    printf("\t\t DATA: %u\n", rudp->flags.DATA);
    printf("\t\t FIN: %u\n", rudp->flags.FIN);

    // Printing other data of the socket
    printf("\tSequal number: %d\n", rudp->sequalNum);
    printf("\tChecksum: %d\n", rudp->checksum);
    printf("\t Length: %d\n", rudp->dataLength);

    printf("\n");
}

int rudp_socket()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0); // Creating the socket in UDP format

    if (sock == -1)
    {
        perror("socket(2)");
        return -1;
    }
    return sock;
}

/****************************************8*/

int rudp_connect(int socket, const char *ip, int port)
{
    if (set_time(socket, 1) == -1)
    {
        return -1;
    }

    // Setting up the address in the socket struct like in TCP protocol
    struct sockaddr_in serveradd;
    memset(&serveradd, 0, sizeof(serveradd));

    serveradd.sin_family = AF_INET;  // for ipv4 address
    serveradd.sin_port = hton(port); // we're getting the port from outside

    int val = inet_pton(AF_INET, ip, &serveradd.sin_addr);

    if (val <= 0)
    {
        // printf("");
        perror("inet_pton failed");
        return -1;
    }

    if (connect(socket, (struct sockaddrr *)&serveradd, sizeof(serveradd)) == -1)
    { // if the connection will fail
        perror("Connetion failed");
        return -1;
    }

    RUDP *rudp = malloc(sizeof(RUDP)); // creating the struct and allocating memory for getting the data
    memset(rudp, 0, sizeof(RUDP));     // setting the whole data to 0
    rudp->flags.SYN = 1;               // setting the syncronzie to 1
    int tries = 0;                     // checking how many reis we had for sending the data
    RUDP *recv = NULL;                 // For getting the packets

    while (tries < 3) // 3 will be the amount of tries, that if is bigger we'll try sending again
    {
        int sendRes = sendto(socket, rudp, sizeof(RUDP), 0, NULL, 0); // using sendto of UDP protocol
        if (sendRes == -1)                                            // means faild to sent the data
        {
            perror("failed to send");
            free(rudp); // freeing the allocated memory
            return -1;  // returning error number
        }

        // Opening the clock
        clock_t start = clock();

        // trying to get the data
        do
        {
            recv = malloc(sizeof(RUDP));
            memset(recv, 0, sizeof(RUDP));
            // trying to get the data
            int get_data_res = recvfrom(socket, recv, sizeof(RUDP), 0, NULL, 0); // UDP receiving data

            if (get_data_res == -1) // means the we've failed getting the data
            {
                perror("failed to recive data");
                // need to free both struct
                free(rudp);
                free(recv);
                return -1;
            }

            if (recv->flags.SYN && recv->flags.ACK) // if they are both 1, means true, we have a connection
            {                                       // The connection stays in the socket now
                printf("Are connected\n");
                free(rudp);
                free(recv);
                return 1;
            }
            else
            {
                printf("worng packet received in the connection");
            }
        } while ((double)(clock() - start) / CLOCKS_PER_SEC < 1); // 1 second is our timeout
        tries++;                                                  // Incrementing the total tries number
    }

    prinf("Couldn't connect");
    free(rudp);
    free(recv);
    return 0; // return 0 for faild to connect
}

/*************************************************/

// Here will try to bind the connection
int rudp_get_con(int socket, int port)
{
    // Setup the server and handshake as before
    struct sockaddr_in serveradd;
    memset(&serveradd, 0, sizeof(serveradd));

    // Putting the IP address an port to the socket
    serveradd.sin_family = AF_INET; // Ipv4
    serveradd.sin_port = htons(port);
    serveradd.sin_addr.s_addr = htonl(INADDR_ANY); // biding the socket to any available network

    int bind_res = bind(socket, (struct sockaddr *)&serveradd, sizeof(serveradd));

    if (bind_res == -1) // means the bind failed
    {
        perror("failed binding");
        close(socket); // closing the socket
        return -1;     // for error
    }

    // receiving SYN
    struct sockaddr_in clientadd;
    socklen_t clientaddLen = sizeof(clientadd);
    memset((char *)&clientadd, 0, sizeof(clientadd));

    RUDP *rudp = malloc(sizeof(RUDP));
    memset(&rudp, 0, sizeof(RUDP));

    int recv_length_bytes = recvfrom(socket, rudp, sizeof(RUDP) - 1, 0, (struct sockaddr *)&clientadd, &clientaddLen);

    if (recv_length_bytes == -1) // recvfrom failed
    {
        perror("failed recvfrom");
        free(rudp);
        return -1; // Error sign
    }

    if (connect(socket, (struct sockaddr *)&clientadd, &clientaddLen) == -1) // connection failed
    {
        perror("connect function failed");
        free(rudp); // free after allocated memory
        return -1;
    }

    if (rudp->flags.SYN == 1)
    {
        RUDP *reply = malloc(sizeof(RUDP));
        memset(rudp, 0, sizeof(RUDP));
        // Setting the flags to 1
        reply->flags.SYN = 1;
        reply->flags.ACK = 1;

        int send_res = sendto(socket, reply, sizeof(RUDP), 0, NULL, 0);

        if (send_res == -1) // means sending failed
        { 
            perror("sendto func' failed");
            free(rudp);
            free(reply);
            return -1;
        }

        set_time(socket, 1 * 10);
        free(rudp);
        free(reply);
        return 1; // Succeeded to get the conncetion of the client 
    }
    return 0; // for unsuccessing
}

/**************************************/

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

int rudp_close(int socket)
{
    close(socket);
}

// -1 for errror, 1 for success
int set_time(int socket, int time)
{

    // Setting for the socket a timeout, using timeval struct
    struct timeval timeout;
    timeout.tv_sec = time;
    timeout.tv_usec = 0;

    if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        perror("Error setting time out for the socket");
        return -1;
    }
    return 1;
}
