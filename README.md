asyncserver
===========
This is a small project developed to explore issues involved with getting an
asynchronous HTTP server running using cpp-netlib. All it does is
asynchronously capture the message post of POSTed input and log its progress.

Requirements
------------
There's really just three C++ files in the project, plus a CMake 2.8 file.
You'll need to install libcppnetlib-dev and g++ to compile it.

Usage
-----
Launch the server on the command line. It writes its log output to stdout.

Test the server by POSTing data to it. For example:

> rcook$ curl -H "Content-Type: application/json" -X POST -d @AsyncDaemon.cpp http://localhost:8787/foo

The server will receive the file (its own source code in this case) and log its progress as it reads in the request. It generates a simple output message.

