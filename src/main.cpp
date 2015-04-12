#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

//will parse the commands passed in 
//and will output a vector of char**
//vector<char**> parseline(char** line) {
	//NOT SURE IF I WILL KEEP THIS
//}

void runexec(char** argv);	//will use system calls to run the 
							//commands provided

char findconnect(int cnt, char** str);	//will determine the next connector

int findchar(int cnt, char** line, char c);
//finds a char index from a char**

int main(int argc, char** argv) {
	string input;
	while(input!="exit") {
		cout << "$ ";
		getline(cin, input);
		//cout << "output: " << findconnect(argc, argv) << endl;

	}
	//first we need to parse once by 
	//using find() to determine the closest
	//connector to the current command
	//
	//then we take the token and execute it
	//
	//then we check the connector to see if we
	//continue running commands


	return 0; 
}

//will use system calls to run the commands provided
void runexec(char** argv) {

}

char findconnect(int cnt, char** str) {
	int orcon = findchar(cnt, str, '|'); 
	cout << "line" << endl;
	int andcon = findchar(cnt, str, '&'); 
	int semicon = findchar(cnt, str, ';');
	
	if(orcon==-1 && andcon==-1 && semicon==-1) {
		return '1';		//if all equal to -1, it means that 
	}					//there are no more connectors

	if(orcon<andcon) { //checks which connector is the closest
		if(orcon<semicon) 
			return '|';
		else
			return ';';
	}
	else if(andcon<semicon) 
			return '&';
		else
			return ';';
}

int findchar (int cnt, char** line, char c) {
	for(int i=0; i<cnt; i++) {
		char* str = line[i];
		for(int j=0; str[j]!=0; j++) {
			cout << str[j] ;
			if(str[j]==c)
				return i;
		}
		cout << endl;
	}
	return -1;
}
