all: bin rshell ls rm
	
rshell: bin
	g++ -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell

ls: bin
	g++ -g -Wall -Werror -ansi -pedantic src/ls.cpp -o bin/ls

rm: bin
	g++ -g -Wall -Werror -ansi -pedantic src/rm.cpp -o bin/rm

bin: 
	mkdir bin
