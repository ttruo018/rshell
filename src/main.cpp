#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <queue>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

using namespace std;
using namespace boost;

void runexec(char** argv);	//will use system calls to run the 
							//commands provided

string findconnect(string str);		//outputs a string of every connector

queue<string> parseline(string str);		//separate commands by connectors from the original string

void parsecommand(char** &cstrarray, string str);	//parse the spaces from each command and output as char*

//vector<string> parsecommand(string str);	//parse the spaces from each command and output as a string

//vector<const char* > parsecommand(string str);	//parse the spaces from each command and output as char*

//void parsecommand(vector<const char* > arg, string str);	//parse the spaces from each command and output as char*


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
			char** cstr = new char*[cmds.front().size()];
			for(unsigned int i=0; i<cmds.front().size(); ++i) {
				cstr[i] = (char*) malloc (cmds.front().size());
			}
			parsecommand(cstr, cmds.front());
			runexec(cstr);
			//vector<const char* > param = parsecommand(cmds.front());
			//vector<const char* > param(cmds.front().size()+1);
			//parsecommand(param, cmds.front());
			//for(unsigned int i=0; i<param.size(); ++i) {
				//for(unsigned int j=0; (param[i][j])!='\0'; ++j) {
//					cout << "param: " << (param[i][j]) << endl;
				//}
				//execvp(param[0],param);
				//perror("Didn't work");
			//}
			//vector<string> test;
			//test.push_back("ls ");
			//test.push_back("-a ");
			//test.push_back("-l");
			//char** troll = new char*[test.size()*2];
			//cout << "test.size() : " << test.size() << endl;
			//for(unsigned i=0; i< test.size(); ++i) {
			//	cout << "Errror at " << i << endl;
			//	cout << "test[" << i << "] : " << test[i] << endl;
			//	strcpy(troll[i],test[i].c_str());
			//	cout << "troll[" << i << "] : " << troll[i] << endl;
			//}
			//execvp(test[0], test);

			cmds.pop();
			//for(unsigned int i=0; i<cmds.front().size(); ++i) {
			//	delete test[i];
			//}
			//delete [] test;
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
	int pid = fork();
	if(pid==-1 ) {	//meaning fork() returned an error
		perror("Error with fork().");
		exit(1);
	}
	else if(pid==0) {	//meaning we're in the child process
		cout << "This is the child process." << endl;
		cout << "argv[0] : " << argv[0] << endl;
		if(execvp(argv[0], argv)) {
			perror("There was an error in execvp." );
		}
		exit(1);
	}
	else if(pid>0) {	//meaning we're in the parent process
		if(wait(0)==-1) {
			perror("There was an error with wait()." );
		}
	}
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

void parsecommand(char** &cstrarray, string str) {
	vector<string> vecstr;
	char_separator<char> delim(" ");
	tokenizer< char_separator<char> > token(str, delim);
	unsigned int in =0;
	for(tokenizer< char_separator<char> >::iterator i=token.begin(); i!=token.end(); ++i) {
		vecstr.push_back(*i);
		++in;
	}
	for(unsigned int j=0; j<vecstr.size(); ++j) {
		strcpy(cstrarray[j], vecstr[j].c_str());
	}
	cstrarray[vecstr.size()] = '\0';
}

//vector<string> parsecommand(string str) {
//	vector<string> out;
//	char_separator<char> delim(" ");
//	tokenizer< char_separator<char> > token(str, delim);
//	for(tokenizer< char_separator<char> >::iterator i=token.begin(); i!=token.end(); ++i) {
//		out.push_back(*i);
//		cout << "*i: " << *i << endl;
//	}
//	return out;
//}

//vector<const char* > parsecommand(string str) {
//void parsecommand(vector<const char* > arg, string str) {
//	vector<string> strarray;
//	char_separator<char> delim(" ");
//	tokenizer< char_separator<char> > token(str, delim);
//	for(tokenizer< char_separator<char> >::iterator i=token.begin(); i!=token.end(); ++i) {
//		strarray.push_back(*i);
//	}
//	for(unsigned int i=0; i<strarray.size(); ++i) {
//		arg[i] = &(strarray[i][0]);
//	}
//	arg[strarray.size()] = NULL;
//}

