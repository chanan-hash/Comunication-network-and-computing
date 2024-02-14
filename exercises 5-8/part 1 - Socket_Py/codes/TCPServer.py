#import socket
from socket import *

SERVER_ADDRESS = ('', 13000)
# serverSocket = socket.socket()
serverSocket = socket(AF_INET, SOCK_STREAM)

serverSocket.bind(SERVER_ADDRESS)
serverSocket.listen(1)
print("The server is ready to receive client")
while True:
    connectionSocket, addrClient = serverSocket.accept()
    # sentence = connectionSocket.recv(4096)
    sentence = connectionSocket.recv(4096).decode()

    print("Get from client ",addrClient, ":", sentence)
    capitalizedSentence = sentence.upper()
    connectionSocket.send(bytes(capitalizedSentence.encode()))
    # connectionSocket.send(capitalizedSentence).encode()
    connectionSocket.close()