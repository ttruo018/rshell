#include <unistd.h>
#include <string>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <queue>

using namespace std;
using namespace boost;

//will parse the commands passed in 
//and will output a vector of char**
//vector<char**> parseline(char** line) {
	//NOT SURE IF I WILL KEEP THIS
//}

void runexec(char** argv);	//will use system calls to run the 
							//commands provided

string findconnect(string str);		//outputs a string of every connector

queue<string> parseline(string str);		//separate commands by connectors from the original string

//char* parsecommand(string str);	//parse the spaces from each command and output as char*

int main(int argc, char** argv) {
	string input;
	bool end = false;
	while(!end) {
		cout << "$ ";
		getline(cin, input);
		string connect = findconnect(input);	//connect stores every connector
		queue<string> cmds = parseline(input);
		while(!cmds.empty()) {
			cout << "command: " << cmds.front() << endl;
			cmds.pop();
		}
			
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

string findconnect(string str) {
	string out;
	int orcon = str.find("||"); 
	int andcon = str.find("&&"); 
	int semicon = str.find(";");
	
	if(orcon==-1 && andcon==-1 && semicon==-1)	//if all equal to -1, it means that 
		return "";								//there are no more connectors

	if(andcon==-1) {	//if there is no "&&"
		if(semicon==-1) {
			out.push_back('|');
			str.erase(orcon,2);
		}
		else if(orcon==-1) {
			out.push_back(';');
			str.erase(semicon,1);
		}
		else if(orcon<semicon) {
			out.push_back('|');
			str.erase(orcon,2);
		}
		else {
			out.push_back(';');
			str.erase(semicon,1);
		}
	}
	else if(semicon==-1) {	//if there is no ";"
		if(andcon==-1) {
			out.push_back('|');
			str.erase(orcon,2);
		}
		else if(orcon==-1) {
			out.push_back('&');
			str.erase(andcon,2);
		}
		else if(orcon<andcon) {
			out.push_back('|');
			str.erase(orcon,2);
		}
		else {
			out.push_back('&');
			str.erase(andcon,2);
		}
	}
	else if(orcon==-1) {	//if there is no "||"
		if(andcon==-1) {
			out.push_back(';');
			str.erase(semicon,1);
		}
		else if(semicon==-1) {
			out.push_back('&');
			str.erase(andcon,2);
		}
		else if(andcon<semicon) {
			out.push_back('&');
			str.erase(andcon,2);
		}
		else {
			out.push_back(';');
			str.erase(semicon,1);
		}
	}
	else if(orcon<andcon) { //checks which connector is the closest if all are inputted
		if(orcon<andcon) {
			if(orcon<semicon) {
				out.push_back('|');
				str.erase(orcon,2);
			}
			else {
				out.push_back(';');
				str.erase(semicon);
			}
		}
	}
	else if(andcon<semicon) {
		out.push_back('&'); str.erase(andcon,2);
	}
	else {
		out.push_back(';');
		str.erase(semicon,1);
	}
	out = out + findconnect(str);
	return out;
}

queue<string> parseline(string str) {
	queue<string> out;
	char_separator<char> delim(";&|");
	tokenizer< char_separator<char> > token(str, delim);
	
	for(tokenizer< char_separator<char> >::iterator i=token.begin(); i!=token.end(); ++i) {
		out.push(*i);
	}
	return out;
}

//char** parsecommand(string str) {
//	char** out;	
//	tokenizer<> token(str);
//	for(tokenizer<>::iterator i=token.begin(); i!=token.end(); ++i) {
//		
//	}
//}
