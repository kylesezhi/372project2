TESTING MACHINE
--------------------------------------------------------------------------------
flip2.engr.oregonstate.edu

COMPILE
--------------------------------------------------------------------------------
Run the "compile.sh" bash shell script:
$ ./compile.sh

RUN
--------------------------------------------------------------------------------
Start the server:
$ ./ftserver 12012

List files from the client:
$ python ftclient.py 12012 12000 -l

Get a file from the server (put ftclient.py in another folder):
$ python ftclient.py 12012 12000 -g README.txt

You can also get help on how to use the client's arguments:
$ python ftclient.py -h
