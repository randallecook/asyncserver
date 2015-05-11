asyncserver
===========
This is a small project developed to explore issues involved with getting an
asynchronous HTTP server running using cpp-netlib. All it does is
asynchronously capture the message body of POSTed input and log its progress as
it does so.

Requirements
------------
There are really just three C++ files in the project, plus a CMake 2.8 file. It
was developed on, and targeted at, Ubuntu 14.04. You'll need to install the
libcppnetlib-dev and g++ packages to compile it.

Usage
-----
Launch the server on the command line. It writes its log output to stdout.

Test the server by POSTing data to it. For example:

> rcook$ curl -H "Content-Type: application/json" -X POST -d @AsyncDaemon.cpp http://localhost:8787/foo

The server will receive the file (its own source code in this case) and log its progress as it reads in the request. It generates a simple output message.

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

