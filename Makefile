server: server.o
	g++ -O2 server.o -o server -pthread
	mkdir -p Simulator/output
	echo "Successfully compiled the program. Now execute it with './server'"

server.o: server.cpp
	g++ -c server.cpp -o server.o -g

clean:
	rm -f server server.o
