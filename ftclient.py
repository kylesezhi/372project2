from socket import *
import sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("serverhost", help="server name, e.g.: flip2.engr.oregonstate.edu")
parser.add_argument("serverport", type=int, help="control port number, e.g.: 12012")
parser.add_argument("dataport", type=int, help="data port number, e.g.: 12013")

group = parser.add_mutually_exclusive_group()
group.add_argument("--list","-l", help="list all files on the server", action="store_true")
group.add_argument("--get","-g", nargs=1, help="get file specified by GET")
args = parser.parse_args()
# print args.serverhost
    
clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.connect((args.serverhost, args.serverport))
if args.list:
    clientSocket.send("-l")
else:
    clientSocket.send("-g")
    
response = clientSocket.recv(1024)
print(response)
clientSocket.close()
