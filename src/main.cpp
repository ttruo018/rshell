//THINGS TO DO:
//

#include <unistd.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <queue>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

using namespace std;
using namespace boost;

bool runexec(vector<string> argv);	//will use system calls to run the commands provided and returns only 0 when sucessful

bool runio(queue<string> &cmds, queue<string> &connect);

string findconnect(string str);		//outputs a string of every connector

queue<string> findio(string str);			//outputs a string of each io redirection or pipe

queue<char> strtoq(string connect);		//converts the string of connectors to a queue

queue<string> parseline(string str, bool iomode);	//separate commands by connectors from the original string

vector<string> parsecommand(bool &end, string str);

string checkcomment(string str);		//checks and takes out comments on the command line

void printinfo();		//prints the username and machine name before the prompt

bool iomode(string line);

int main(int argc, char** argv) {
	string input;
	bool end = false;
	while(!end) {
		printinfo();
		cout << "$ ";
		getline(cin, input);
		input = checkcomment(input);
		string constr;			
		queue<string> iosym;
		queue<char> connect;		//connect stores every connector
		queue<string> cmds;

		bool redirect = iomode(input);
		if(redirect) {
			//PARSE BY '>', '<', AND '|' CHARACTERS
			cout << "io redirection mode" << endl;

			iosym = findio(input);			
			int iosymsize = iosym.size();
			cmds = parseline(input, redirect);
			int cmdsize = cmds.size();
			if(iosymsize >= cmdsize) {
				cout << "Error: Too many IO redirection symbols" << endl;
			}
			else {
				end = runio(cmds, iosym);
			}
		}
		else {
			//PARSE BY '&', '|', AND ';' CHARACTERS
			cout << "connector mode" << endl;
			constr = findconnect(input);			
			connect = strtoq(constr);		//connect stores every connector
			cmds = parseline(input, redirect);

			bool cmdend = false;		//tells if a command should end
			bool cmdfail = false;		//tracks if a command has failed
			while(!cmds.empty() && !cmdend && !end) {
				cmdend = false;
				vector<string> cstr;
				cstr = parsecommand(end, cmds.front());
				if(!end) {
					cmdfail = runexec(cstr);		//cmdfail tells if the command failed, 0 if failed, 1 if not
					cmds.pop();
					char next = connect.front();
					connect.pop();
					if(next=='&' && !cmdfail) {		//if command fails and next connector is &&,
						cmdend = true;				// then no other remaining commands are run
					}
					if(next=='|' && cmdfail) {
						cmdend = true;
					}
				}
			}
		
		}
			
	}
	cout << "Exiting from rshell." << endl;
	return 0; 
}

//will use system calls to run the commands provided
bool runexec(const vector<string> argv) {
	char** cmd = new char*[argv.size()+1];
	for(unsigned int i=0; i<argv.size(); ++i) {
		cmd[i] = const_cast<char* >((argv[i]).c_str());
	}
	cmd[argv.size()] = '\0';
	int pid = fork();
	int status;
	if(pid==-1 ) {	//meaning fork() returned an error
		perror("Error with fork(). ");
		delete[] cmd;
		exit(1);
	}
	else if(pid==0) {	//meaning we're in the child process
		if(execvp(cmd[0], cmd)==-1) {
			perror("There was an error in execvp. " );
		}
		delete[] cmd;
		exit(1);
	}
	else if(pid>0) {	//meaning we're in the parent process
		if(wait(&status)==-1) {
			perror("There was an error with wait(). " );
			delete[] cmd;
			exit(1);
		}
	}
	delete[] cmd;
	return (status) ? false : true;
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

queue<string> parseline(string str, bool iomode) {
	queue<string> out;
	char something[4];
	if(iomode) {
		something[0] = '<';	
		something[1] = '>';	
		something[2] = '|';	
	}
	else {
		something[0] = '&';	
		something[1] = ';';	
		something[2] = '|';	
	}
	char_separator<char> delim(something);
	tokenizer< char_separator<char> > token(str, delim);
	
	for(tokenizer< char_separator<char> >::iterator i=token.begin(); i!=token.end(); ++i) {
		out.push(*i);
	}
	return out;
}

vector<string> parsecommand(bool &end, string str) {
	end = false;
	vector<string> vecstr;
	char_separator<char> delim(" ");
	tokenizer< char_separator<char> > token(str, delim);
	unsigned int cnt = 0;
	for(tokenizer< char_separator<char> >::iterator i=token.begin(); i!=token.end(); ++i) {
		if(*i=="exit") {
			end = true;
			return vecstr;
		}
		string tmp = *i;
		vecstr.emplace_back(tmp);
		cnt++;
	}
	return vecstr;
}

queue<char> strtoq(string connect) {
	queue<char> out;
	for(unsigned i=0; i<connect.size(); ++i) {
		out.push(connect[i]);
	}
	return out;
}

string checkcomment(string str) {
	int index = str.find("#");
	if(index==-1) {
		return str;
	}
	else {
		return str.substr(0,index);
	}
}

void printinfo() {
	if(getlogin()==NULL) {
		perror("Could not find username. ");
	}
	else {
		cout << getlogin();
	}
	int len = 64;
	char* machine = new char[len];
	if(gethostname(machine, len)==-1) {
		perror("Could not find machine name. ");
		delete[] machine;
	}
	else {
		cout << "@" << machine;
	}
	delete[] machine;
}

bool iomode(string line) {
	int outdirect = line.find('>');
	int indirect = line.find('<');
	int pipe = line.find('|');
	int andconnect = line.find('&');
	int semiconnect = line.find(';');
	if(-1!=outdirect || -1!=indirect) {
		return true;
	}
	else if(-1!=pipe && (-1==andconnect || -1==semiconnect)) {
		int secondpipe = line.find('|', pipe+1);
		if(secondpipe==(pipe+1)) {
			return false;
		}
		else {
			return true;
		}
	}
	else {
		return false;
	}
}

queue<string> findio(string str) {
	queue<string> out;
	int pipe = str.find("|"); 
	int left = str.find("<"); 
	int right = str.find(">");
	int strsize = str.size();
	
	if(left==-1 && pipe==-1 && right==-1) {	//if all equal to -1, it means that 
		queue<string> emptyq;
		return emptyq;								//there are no more connectors
	}

	if(pipe==-1) {	//if there is no "|"
		if(right==-1) {
			out.push("<");
			str.erase(left,1);		//CHANGE to support multiple > and <
		}
		else if(left==-1) {
			if(right!=(strsize-1) && str.at(right+1)=='>') {
				out.push(">>");
				str.erase(right,2);
			}	
			else {
				out.push(">");
				str.erase(right,1);
			}
		}
		else if(left<right) {
			out.push("<");
			str.erase(left,1);
		}
		else {
			if(right!=(strsize-1) && str.at(right+1)=='>') {
				out.push(">>");
				str.erase(right,2);
			}
			else {
				out.push(">");
				str.erase(right,1);
			}
		}
	}
	else if(right==-1) {	//if there is no ">"
		if(pipe==-1) {
			out.push("<");
			str.erase(left,1);
		}
		else if(left==-1) {
			out.push("|");
			str.erase(pipe,1);
		}
		else if(left<pipe) {
			out.push("<");
			str.erase(left,1);
		}
		else {
			out.push("|");
			str.erase(pipe,1);
		}
	}
	else if(left==-1) {	//if there is no "<"
		if(pipe==-1) {
			if(right!=(strsize-1) && str.at(right+1)=='>') {
				out.push(">>");
				str.erase(right,2);
			}
			else {
				out.push(">");
				str.erase(right,1);
			}
		}
		else if(right==-1) {
			out.push("|");
			str.erase(pipe,1);
		}
		else if(pipe<right) {
			out.push("|");
			str.erase(pipe,1);
		}
		else {
			if(right!=(strsize-1) && str.at(right+1)=='>') {
				out.push(">>");
				str.erase(right,2);
			}
			else {
				out.push(">");
				str.erase(right,1);
			}
		}
	}
	else if(left<pipe) { //checks which connector is the closest if all are inputted
		if(left<pipe) {
			if(left<right) {
				out.push("<");
				str.erase(left,1);
			}
			else {
				if(right!=(strsize-1) && str.at(right+1)=='>') {
					out.push(">>");
					str.erase(right,2);
				}
				else {
					out.push(">");
					str.erase(right,1);
				}
			}
		}
	}
	else if(pipe<right) {
		out.push("|");
		str.erase(pipe,1);
	}
	else {
		if(right!=(strsize-1) && str.at(right+1)=='>') {
			out.push(">>");
			str.erase(right,2);
		}
		else {
			out.push(">");
			str.erase(right,1);
		}
	}
	queue<string> next = findio(str);
	while(!next.empty()) {
		out.push(next.front());
		next.pop();
	}
	return out;

}

bool runio(queue<string> &cmds, queue<string> &iosym) {
	bool endprog = false;
	bool endcmd = false;
	
	while(!iosym.empty() && !endcmd) {
		if(iosym.front() == "<") {
			cout << "iosym = < " << endl;		//DELETE
			iosym.pop();
			vector<string> cmd1 = parsecommand(endprog, cmds.front());
			cmds.pop();
			if(!endprog && cmds.empty()) {		//if nothing on the right side of <
				cout << "Error : need a filename on the right of '<'" << endl;
				endcmd = true;
			}
			else if(!endprog){
				vector<string> cmd2 = parsecommand(endprog, cmds.front());
				if(cmd2.empty()) {
					cout << "Error : need a filename on the right of '<'" << endl;
					endcmd = true;
					cmds.pop();
				}
				else {
					//DO NORMAL OPERATION HERE
					cmds.pop();
				}
			}
		}
		else if(iosym.front() == ">" || iosym.front() == ">>") {
			cout << "iosym = ";		//DELETE
			(iosym.front()==">") ? cout << "> " : cout << ">>";	//DELETE
			cout << endl;		//DELETE
			iosym.pop();
		}
		else if(iosym.front() == "|") {
			cout << "iosym = | " << endl;		//DELETE
			iosym.pop();
		}
	}

	return endprog;
}
