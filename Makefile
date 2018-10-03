linker.o : linker.cpp
	g++ -std=c++11 -c linker.cpp

linker : linker.o
	g++ -std=c++11 -o linker linker.o
clean: 
	rm -f linker