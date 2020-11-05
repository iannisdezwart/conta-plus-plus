index: index.o
	g++ -O3 index.o -o index `libpng-config --ldflags` -pthread
	mkdir -p output
	echo "Successfully compiled the program. Now execute it with './index'"

index.o: index.cpp
	g++ -c index.cpp -o index.o `libpng-config --cflags`

clean:
	rm -f index index.o
