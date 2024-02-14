# Use the gcc compiler.
CC = gcc

# Flags for the compiler.
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic

# Command to remove files.
RM = rm -f

# Phony targets - targets that are not files but commands to be executed by make.
.PHONY: all default clean run_tcp_server runtc runts runuc runus

# Default target - compile everything and create the executables and libraries.
all: tcp_server tcp_client udp_server udp_client

# Alias for the default target.
default: all


############
# Programs #
############

# Compile the tcp server.
tcp_server: tcp_server.o
	$(CC) $(CFLAGS) -o $@ $^

# Compile the tcp client.
tcp_client: tcp_client.o
	$(CC) $(CFLAGS) -o $@ $^

# Compile the udp server.
udp_server: udp_server.o
	$(CC) $(CFLAGS) -o $@ $^

# Compile the udp client.
udp_client: udp_client.o
	$(CC) $(CFLAGS) -o $@ $^


################
# Run programs #
################

# Run tcp server.
runts: tcp_server
	./tcp_server

# Run tcp client.
runtc: tcp_client
	./tcp_client

# Run udp server.
runus: udp_server
	./udp_server

# Run udp client.
runuc: udp_client
	./udp_client


################
# System Trace #
################

# Run the tcp server with system trace.
runts_trace: tcp_server
	strace ./tcp_server

# Run the tcp client with system trace.
runtc_trace: tcp_client
	strace ./tcp_client

# Run the udp server with system trace.
runus_trace: udp_server
	strace ./udp_server

# Run the udp client with system trace.
runuc_trace: udp_client
	strace ./udp_client

################
# Object files #
################

# Compile all the C files into object files.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


#################
# Cleanup files #
#################

# Remove all the object files, shared libraries and executables.
clean:
	$(RM) *.o *.so tcp_server tcp_client udp_server udp_client