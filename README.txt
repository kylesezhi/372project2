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

Get a file from the server:
$ python ftclient.py 12012 12000 -g README.txt

You can also see the usage of the client:
$ python ftclient.py -h
