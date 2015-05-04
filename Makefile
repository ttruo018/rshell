all: bin rshell ls rm mv
	
rshell: bin
	g++ -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell

ls: bin
	g++ -g -Wall -Werror -ansi -pedantic src/ls.cpp -o bin/ls

rm: bin
	g++ -g -Wall -Werror -ansi -pedantic src/rm.cpp -o bin/rm

mv: bin
	g++ -g -Wall -Werror -ansi -pedantic src/mv.cpp -o bin/mv

bin: 
	mkdir bin
