#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // For the memset function
#include <arpa/inet.h>  // For the in_addr structure and the inet_pton function
#include <sys/socket.h> // For the socket function
#include <unistd.h>     // For the close function
#include <sys/time.h>   // For tv struct
#include <time.h>
#include "RUDP_API.h"

#define PORT 5061 // may be changed, temporary port for now

#define BUFFER_SIZE 1024 // need to be changed to 2mb

/***Will act as an server***/

int main(int argc, char *argv[])
{
    // Checking if the command is right
    if (argc != 3 || strcmp(argv[i], "-p") != 0)
    {
        printf("usage: %s -p <port>\n", argv[0]);
    }

    printf("Start receiver\n");
    int port = atoi(argv[2]); // according to the format it's supposed to have there the port number

    int socket = rudp_socket();
    if (socket == -1)
    {
        printf("Socket couldn't be created\n");
        return -1;
    }

    printf("Socket has been created succeesfuly!\n");
    printf("waitnig for RUDP connection...\n");

    int val = rudp_get_con(socket, port); // getting the connection

    if (val == 0)
    {
        printf("Couldn't get a connection\n");
        return -1;
    }

    printf("Connection request received\n");

    /*
     opening a file to keep the data instead of a dynamic array that needs to be reallocated all the time
     Opens a file in read and write mode.
     It creates a new file if it does not exist, if it exists,
     it erases the contents of the file and the file pointer starts from the beginning.
    */
    FILE *data_file = fopen("stats", "w+");
    if (data_file == NULL)
    {
        printf("Error opening the file\n");
        return 1; // error
    }

    printf("Sender connected and data is received to a file");

    fprintf(data_file, "\n\n --------------- Stats ----------\n");
    double avgTime = 0;
    double avgSpeed = 0;
    clock_t start, finish;

    char *recv_data = NULL;
    int data_len = 0;
    char total_size[2097152] = {0}; // 2mb, and initializing the array with 0

    val = 0;
    int run = 1;

    start = clock(); // opening the clock
    finish = clock();

    do
    {
        val = rudp_receive(socket, &recv_data, &data_len);

        if (val == -2) // means sender closed the connection
        {
            break;
        }

        else if (val == -1)
        {
            printf("Error receiving the data");
            return -1;
        }

        else if (val == 1 && start < finish) // got the data in the fiest one, starting the timer again
        {
            start = clock(); // getting the time again
        }

        else // (val == 1)
        {
            strcat(total_size, recv_data);
            printf("received total: %zu\n", sizeof(total_size));

            finish = clock(); // getting the time to calculate the time for stats
            double how_long = ((double)(finish - start)) / CLOCKS_PER_SEC;
            avgTime += how_long;

            double speed = 2 / how_long;
            avgSpeed += speed;

            fprintf(data_file, "Run #%d Data: Time=%f s' Speed=%f MB/s\n", run, how_long, speed);

            memset(total_size, 0, sizeof(total_size)); // reseting it for the next income data
            run++;                                     // incrementing the run counter
        }
    } while (val >= 0);

    printf("closing connection1\n");

    // adding to our data file more stats
    fprintf(data_file, "\n");
    fprintf(data_file, "Average time: %f s'\n", avgTime / (run - 1));
    fprintf(data_file, "Average time: %f s'\n", avgSpeed / (run - 1));

    fprintf(data_file, "\n\n----------------------------------\n"); // for ending like in the exmple

    rewind(data_file); // setting the pointer to the begining of the file,
                       // to print know the data to the user from the file.

    char buffer[100]; // buffer for getting the text from the file and printing it to the user

    while (fgets(buffer, 100, data_file) != NULL) // while it is not the end of the file aka EOF
    {
        printf("%s", buffer);
    }

    // closing the file and deleting it
    fclose(data_file);
    remove("stats");

    printf("Receiver end\n");
    return 0;
}