FLAGS =  -std=c++11 -g -Wall -Werror -pedantic #-ansi 

all: bin rshell ls rm mv
	
rshell: bin src/main.cpp
	g++ $(FLAGS) src/main.cpp -o bin/rshell

ls: bin src/ls.cpp
	g++ $(FLAGS) src/ls.cpp -o bin/ls

rm: bin src/rm.cpp
	g++ $(FLAGS) src/rm.cpp -o bin/rm

mv: bin src/mv.cpp
	g++ $(FLAGS) src/mv.cpp -o bin/mv

bin: 
	mkdir bin
