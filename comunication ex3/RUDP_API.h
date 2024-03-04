// A header file for RUDP APIvfunctions

#ifndef RUDP_API_H
#define RUDP_API_H
#define MAX_SIZE 60000 // will be for max size of data each time

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

// Creating the socket, return -1 if fails
int rudp_socket();

// Sending the data, returning 1 if succedd 0 if fails
// It'll wait fir some of acknowledgment packet for nowing what the state
int rudp_send(int socket, const char *data, int length);

// Receiving the data from the conncetion returning 1 if succedd 0 if fails
// Putting the data in the char buffern and getting also the length
int rudp_receive(int socket, char **buffer, int *length);

// Close the socket
int rudp_close(int socket);

/**** More help functions ****/
// The idea to make UDP reliable, is to make it like TCP protocol, adding like listen and bind functions

// Opening the connection itself between with the socket and the given ip and port
// This is the 2 handshake, returning 1 if succedd 0 if fails
int rudp_connect(int socket, const char *ip, int port);

// Listening for incoming connection, the other side of the 2 handshake
// returning 1 if succedd 0 if fails
int rudp_get_con(int socket, int port);

/*****Helpers structs for checking the data transfer and for the protocol itself ****/

// We're using here bit field like SYN flag will hae 1 for true and 0 for false, and that will create our flage
struct RUDP_flag
{
    unsigned int SYN : 1; // Synchronize flag to know if someone want to establish connection 
    unsigned int ACK : 1; // Acknowledge this for ensure that we've got the whole data
    unsigned int DATA : 1; // To know if the packet is carring a data payload
    unsigned int FIN : 1; // To know if we've finished to send everything and can finish the connection
};

typedef struct _RUDP
{
    struct RUDP_flag flags;
    int sequalNum;
    int checksum;
    int dataLength;
    char data[MAX_SIZE];
} RUDP;

#endif