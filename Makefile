all: bin rshell ls
	
rshell: bin
	g++ -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell

ls: bin
	g++ -Wall -Werror -ansi -pedantic src/ls.cpp -o bin/ls

bin: 
	mkdir bin
