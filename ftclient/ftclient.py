from socket import *
import sys
import argparse

serverName = 'flip2.engr.oregonstate.edu'

# setup argument parsing
parser = argparse.ArgumentParser()
parser.add_argument("serverhost", help="server name, e.g.: flip2.engr.oregonstate.edu")
parser.add_argument("serverport", type=int, help="control port number, e.g.: 12012")
parser.add_argument("dataport", type=int, help="data port number, e.g.: 12013")

group = parser.add_mutually_exclusive_group()
group.add_argument("--list","-l", help="list all files on the server", action="store_true")
group.add_argument("--get","-g", nargs=1, help="get file specified by GET")
args = parser.parse_args()
    
# setup connections
controlSocket = socket(AF_INET, SOCK_STREAM)
controlSocket.connect((args.serverhost, args.serverport))

clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.bind((serverName, int(args.dataport)))

if args.list:
    controlSocket.send("-l " + str(args.dataport))
    clientSocket.listen(1)
    dataSocket, addr = clientSocket.accept()
    response = dataSocket.recv(1024)
    print(response)
    dataSocket.close()
else: # get
    controlSocket.send("-g " + args.get[0] + " " + str(args.dataport))
    response = controlSocket.recv(1024)
    if len(response) > 2: # server does not say OK so print error
        print(response)
    else:
        clientSocket.listen(1)
        dataSocket, addr = clientSocket.accept()
        f = open(args.get[0],'wb')
        response = dataSocket.recv(1024)
        while(response):
            f.write(response)
            response = dataSocket.recv(1024)
        dataSocket.close()
        f.close()
        print("Transfer complete.")

controlSocket.close()
