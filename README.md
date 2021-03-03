# Conta++

A C++ rewrite of my previous project [ContaJS](https://github.com/iannisdezwart/contajs).

Conta++ is a C++ epidemic simulator.
The code in this repository includes the actual Conta++ API,
and a server + web app to communicate with the API.

![image](https://user-images.githubusercontent.com/38715718/109820325-2e7d3300-7c35-11eb-877b-e8c81fd65460.png)

### Compilation of the server

To compile the server, you must have a modern version of GCC installed on
your computer.

[boost::asio::thread_pool](https://www.boost.org/doc/libs/1_66_0/doc/html/boost_asio/reference/thread_pool.html) is needed as a dependency.
The entire [Boost](https://www.boost.org/users/download/) library can be installed on a debian distribution with:

```
$ sudo apt install libboost-all-dev
```

Compile the server with:

```
$ make clean && make
```

This will create the executable file `server`.
Run the executable and open `http://localhost:1337/` in your browser, where you can find the web app.

### Serverless usage of the Conta++ API

You can also test out the API without the need of a server.
You will, however, need the web application in order to view the runs.

Check the file `Simulator/test.cpp` for an example, which you can compile with:

```
$ cd Simulator/ && g++ test.cpp -o test -O3 -pthread
```
