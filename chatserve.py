from socket import *
import sys

if len(sys.argv) != 2:
    print "usage: chatserve.py portnumber"
    exit()

serverName = 'localhost'
clientHandle = ''
serverPort = int(sys.argv[1])
handle = 'serverHandle'
serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind((serverName, serverPort))
serverSocket.listen(1)
while 1:
    print 'Server is ready to receive'
    connectionSocket, addr = serverSocket.accept()
    while True:
        sentence = connectionSocket.recv(1024)
        if sentence[len(sentence)-5:len(sentence)] == "\quit": break
        clientHandle = sentence.split('>')[0];
        print sentence
        responseSentence = raw_input(handle + '> ')
        if responseSentence == "\quit": 
            connectionSocket.send(responseSentence)
            break
        connectionSocket.send(handle + '> ' + responseSentence)
    connectionSocket.close()
    print(clientHandle + ' left the chat.')
