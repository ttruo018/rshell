all: bin
	g++ -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell
	
bin: 
	mkdir bin
