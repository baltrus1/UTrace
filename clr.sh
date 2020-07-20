mkdir bin
g++ -c src/example.cpp -o bin/example.o
g++ -c src/UTrace/UTrace.cpp -o bin/UTrace.o
ar rcs bin/libutrace.a bin/UTrace.o

g++ -pthread bin/example.o -Lbin/ -lutrace -o bin/linked
./bin/linked
