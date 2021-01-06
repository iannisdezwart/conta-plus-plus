server: server.o
	g++ -O3 server.o -o server -pthread
	mkdir -p Simulator/output
	echo "Successfully compiled the program. Now execute it with './server'"

debug: server.cpp
	g++ -c server.cpp -o server.o -g

server.o: server.cpp
	g++ -c server.cpp -o server.o

clean:
	rm -f server server.o
