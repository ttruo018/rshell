all: bin rshell ls rm mv
	
rshell: bin src/main.cpp
	g++ -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell

ls: bin src/ls.cpp
	g++ -g -Wall -Werror -ansi -pedantic src/ls.cpp -o bin/ls

rm: bin src/rm.cpp
	g++ -g -Wall -Werror -ansi -pedantic src/rm.cpp -o bin/rm

mv: bin src/mv.cpp
	g++ -g -Wall -Werror -ansi -pedantic src/mv.cpp -o bin/mv

bin: 
	mkdir bin
