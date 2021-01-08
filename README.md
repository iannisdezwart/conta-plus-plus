# Conta++

A C++ rewrite of my previous project [ContaJS](https://github.com/iannisdezwart/contajs).

Conta++ is a C++ epidemic simulator.
The code in this repository includes the actual Conta++ API,
and a server as well as a webapp to communicate with it.

### Compilation of the server

To compile the server, you must have a modern version of GCC installed on
your computer. The server and Conta++ API need POSIX threads as a dependency.

Compile the server with:

```
$ make clean && make
```

This will create the executable file `server`.

### Serverless usage of the Conta++ API

You can also test out the API without the need of a server.
You will, however, need the web application in order to view the runs.

Check the file `Simulator/test.cpp`, which you can compile with:

```
$ cd Simulator/ && g++ test.cpp -o test -O3 -pthread
```