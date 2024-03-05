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
int send_acknowledgement(int socket, RUDP *rudp);
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
int rudp_send(int socket, const char *data, int length)
{
    // calculating the number of packets and last packet size
    int number_of_packets = length / MAX_SIZE;
    int last_one_size = length % MAX_SIZE;

    RUDP *rudp = malloc(sizeof(RUDP));
    for (int i = 0; i < number_of_packets; i++)
    {
        memset(rudp, 0, sizeof(RUDP));
        rudp->sequalNum = i;  // according to which packet are we now
        rudp->flags.DATA = 1; // Sending data
        if (i == number_of_packets - 1 && last_one_size == 0)
        {
            rudp->flags.FIN = 1; // means  we've finished sending the whole data
        }
        // copying into the rudp struct the data,, according to the place in the memory (data + i * MAX_SIZE) --> data[i]
        memcpy(rudp->data, data + i * MAX_SIZE, MAX_SIZE); /* Copy N bytes of SRC to DEST.  */
        rudp->dataLength = MAX_SIZE;
        rudp->checksum = checksum(rudp);

        do
        { // sending the message
            int send_res = sendto(socket, rudp, sizeof(RUDP), 0, NULL, 0);
            if (send_res == -1)
            {
                perror("Error sending with sendto");
                return -1;
            }                                                              // wating to send again if we need. This is the part of the reliable UDP
        } while (wait_for_acknowledgement(socket, rudp, clock(), 1) <= 0); // wating to seee if this is the write packet to send
    }

    // dealing with the last packet, it can signs us if we've finished whole the data
    if (last_one_size > 0)
    {
        memset(rudp, 0, sizeof(RUDP));
        rudp->sequalNum = number_of_packets; // last packet
        rudp->flags.DATA = 1;                // Still sending data
        rudp->flags.FIN = 1;                 // finishing sending the whole data

        memcpy(rudp->data, data + number_of_packets * MAX_SIZE, last_one_size); // copying the lats packet data to our struct,
                                                                                // allocating only it's amount, don't need the whole MAX_SIZE, and accessing it by the pointer

        rudp->dataLength = last_one_size;
        rudp->checksum = checksum(rudp);

        // Trying to send it again if needed
        do
        {
            int send_last = sendto(socket, rudp, sizeof(RUDP), 0, NULL, 0);
            if (send_last == -1)
            {
                perror("Failed sendto the last packet");
                free(rudp);
                return -1;
            }
        } while (wait_for_acknowledgement(socket, number_of_packets, clock(), 1) <= 0);
        free(rudp);
    }
    return 1; // for success
}

/**************************************************/

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
    RUDP *close_socket = malloc(sizeof(RUDP));
    memset(close_socket, 0, sizeof(RUDP));
    close_socket->flags.FIN = 1; // Finished so closing the connection
    close_socket->sequalNum = -1;
    close_socket->checksum = checksum(close_socket);

    do
    {
        int res_send = sendto(socket, rudp, sizeof(RUDP), 0, NULL, 0);
        if (res_send == -1)
        {
            perror("Fialed sendto when closing");
            free(close_socket);
            return -1; // for error
        }
    } while (wait_for_acknowledgement(socket, -1, clock(), 1) <= 0);
    close(socket);
    free(rudp);
    return 1; // succeeded to close the socket and freeing our rudp struct
}

/*************************************************/

int checksum(RUDP *rudp)
{
    int sum = 0;
    for (int i = 0; i < 10 && i < MAX_SIZE; i++)
    {
        sum += rudp->data[i];
    }
    return sum;
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

int wait_for_acknowledgement(int socket, int sequal_num, clock_t s, clock_t t)
{
    RUDP *reply = malloc(sizeof(RUDP));
    while ((double)(clock() - s) / CLOCKS_PER_SEC < 1)
    {
        int length_recv = recvfrom(socket, reply, sizeof(RUDP) - 1, 0, NULL, 0);
        if (length_recv == -1)
        {
            free(reply);
            return -1; // for errror
        }
        if (reply->sequalNum == sequal_num && reply->flags.ACK == 1)
        { // meaning this is the packet we're wating for
            free(reply);
            return 1;
        }
    }
    free(reply);
    return 0; // for unsuccess
}

int send_acknowledgement(int socket, RUDP *rudp)
{
    RUDP rudp_ack = malloc(sizeof(RUDP));
    memset(rudp_ack, 0, sizeof(RUDP));
    rudp_ack.flags.ACK = 1;

    if (rudp.flags.FIN == 1)
    { // means finished to send the data
        rudp_ack.flags.FIN = 1;
    }
    if (rudp->flags.DATA == 1)
    {
        rudp_ack.flags.DATA = 1;
    }
    rudp_ack.sequalNum = rudp->sequalNum;
    rudp_ack.checksum = checksum(rudp_ack);

    int res_send = sendto(socket, rudp_ack, sizeof(RUDP), 0, NULL, 0);
    if (res_send == -1)
    {
        perror("Failed sendto the acknowledgement");
        free(rudp_ack);
        return -1; // for error
    }

    free(rudp_ack); // succeeded sendinf the acknowledgement and can free the memory and return 1 for success
    return 1; // for success
}
