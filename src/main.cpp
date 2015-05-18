//THINGS TO DO:
//

#include <unistd.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <queue>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

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
		fflush(stdout);
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
	char* usr;
	if((usr=getlogin())==NULL) {
		perror("Could not find username. ");
	}
	else {
		cout << usr;
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
	int pipe1 = line.find('|');
	int andconnect = line.find('&');
	int semiconnect = line.find(';');
	if(-1!=outdirect || -1!=indirect) {
		return true;
	}
	else if(-1!=pipe1 && (-1==andconnect || -1==semiconnect)) {
		int secondpipe = line.find('|', pipe1+1);
		if(secondpipe==(pipe1+1)) {
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
	int pipe1 = str.find("|"); 
	int left = str.find("<"); 
	int right = str.find(">");
	int strsize = str.size();
	
	if(left==-1 && pipe1==-1 && right==-1) {	//if all equal to -1, it means that 
		queue<string> emptyq;
		return emptyq;								//there are no more connectors
	}

	if(pipe1==-1) {	//if there is no "|"
		if(right==-1) {
			if(left<(strsize-2) && str.substr(left,3)=="<<<") {
				out.push("<<<");
				str.erase(left,3);
			}
			else {
				out.push("<");
				str.erase(left,1);
			}
		}
		else if(left==-1) {
			if(right<(strsize-1) && str.at(right+1)=='>') {
				out.push(">>");
				str.erase(right,2);
			}	
			else {
				out.push(">");
				str.erase(right,1);
			}
		}
		else if(left<right) {
			if(left<(strsize-2) && str.substr(left,3)=="<<<") {
				out.push("<<<");
				str.erase(left,3);
			}
			else {
				out.push("<");
				str.erase(left,1);
			}
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
		if(pipe1==-1) {
			if(left<(strsize-2) && str.substr(left,3)=="<<<") {
				out.push("<<<");
				str.erase(left,3);
			}
			else {
				out.push("<");
				str.erase(left,1);
			}
		}
		else if(left==-1) {
			out.push("|");
			str.erase(pipe1,1);
		}
		else if(left<pipe1) {
			if(left<(strsize-2) && str.substr(left,3)=="<<<") {
				out.push("<<<");
				str.erase(left,3);
			}
			else {
				out.push("<");
				str.erase(left,1);
			}
		}
		else {
			out.push("|");
			str.erase(pipe1,1);
		}
	}
	else if(left==-1) {	//if there is no "<"
		if(pipe1==-1) {
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
			str.erase(pipe1,1);
		}
		else if(pipe1<right) {
			out.push("|");
			str.erase(pipe1,1);
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
	else if(left<pipe1) { //checks which connector is the closest if all are inputted
		if(left<pipe1) {
			if(left<right) {
				if(left<(strsize-2) && str.substr(left,3)=="<<<") {
					out.push("<<<");
					str.erase(left,3);
				}
				else {
					out.push("<");
					str.erase(left,1);
				}
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
	else if(pipe1<right) {
		out.push("|");
		str.erase(pipe1,1);
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

int findstr(string param, string &newstr);
struct piping {
	int fd[2];
};
bool runio(queue<string> &cmds, queue<string> &iosym) {
	bool endprog = false;
	bool endcmd = false;
	const int PIPE_READ = 0;
	const int PIPE_WRITE = 1;
	vector<int> pids;
	vector<piping> pipelist;
	int pipecnt = 0;
	int forkcnt = 0;
	bool completepipe = false;
	int stdin_cp;
	if(-1 == (stdin_cp = dup(0))) {
		perror("Error with dup().");
		exit(1);
	}
	int stdout_cp;
	if(-1 == (stdout_cp = dup(1))) {
		perror("Error with dup().");
		exit(1);
	}
			
	while(!cmds.empty() && !endcmd) {
		bool leftio = false;
		bool leftio3 = false;
		bool rightio = false;
		bool rightio2 = false;
		bool pipeio = false;
		string infiles;
		string instr;
		char tmpfile[10] = {'X','X','X','X','X','X', 0};
		int tmpfd;
		string outfiles;
		string outfiles2;

		vector<string> cmd1 = parsecommand(endprog, cmds.front());
		cmds.pop();
		char** cmd = new char*[BUFSIZ];
		for(unsigned int i=0; i<cmd1.size(); ++i) {
			cmd[i] = const_cast<char* >((cmd1[i]).c_str());
		}
		cmd[cmd1.size()] = '\0';

		if(completepipe && cmds.empty()) {
			int pid = fork();
			completepipe = false;
			if(-1 == pid) {
				perror("Error with fork().");
				exit(1);
			}
			else if(0 == pid) {
				if(-1 == dup2(pipelist.at(pipecnt-1).fd[PIPE_READ], 0)) {
					perror("Error with dup2().");
					exit(1);
				}
				if(-1 == execvp(cmd[0], cmd)) {
					perror("Error with execvp.");
					exit(1);
				}
			}
			else if(0 < pid) {
				forkcnt++;
				pids.emplace_back(pid);
				if(-1 == close(pipelist.back().fd[PIPE_READ])) {
					perror("Error with close.");
					exit(1);
				}

				int status;
				for(unsigned int i=0; i<pids.size(); ++i) {
					if(-1 == waitpid(pids.at(i),&status,WNOHANG)) {
						perror("Error with wait().");
						exit(1);
					}
				}
				
				if(-1 == dup2(stdin_cp, 0)) {
					perror("Error with dup2().");
					exit(1);
				}
				if(-1 == dup2(stdout_cp, 1)) {
					perror("Error with dup2().");
					exit(1);
				}
				pipecnt = 0;
			}
		}
		else {

		while(!endcmd && !pipeio && !cmds.empty()) {
			vector<string> nextfiles = parsecommand(endprog, cmds.front());
			if(iosym.front() == "<") {
				iosym.pop();
				cmds.pop();
				leftio = true;
				infiles = nextfiles.back(); 

			}
			else if(iosym.front() == "<<<") {
				iosym.pop();
				leftio3 = true;
				int findstrerr = findstr(cmds.front(), instr);
				if(-1 == findstrerr) {
					cerr << "No string entered." << endl;
					endcmd = true;
				}
				else if(1 == findstrerr) {
					cerr << "No ending quotation." << endl;
					endcmd = true;
				}
				else if(0 == findstrerr) {
					char* istr = const_cast<char* >(instr.c_str());
					char ibuf[BUFSIZ] = {0};
					strcpy(ibuf, istr);
					if(-1 == (tmpfd = mkstemp(tmpfile))) {
						perror("Error with mkstemp().");
						exit(1);
					}
					if(-1 == write(tmpfd, ibuf, BUFSIZ)) {
						perror("Error with write(). (<<<)");
						exit(1);
					}
					if(-1 == close(tmpfd)) {
						perror("Error with close().");
						exit(1);
					}
					cmd[cmd1.size()] = tmpfile;
				}

				cmds.pop();
			}
			else if(iosym.front() == ">" || iosym.front() == ">>") {
				if(iosym.front()==">") {
					rightio = true;
					outfiles = nextfiles.back();
				}
				else {
					rightio2 = true;
					outfiles2 = nextfiles.back();
				}
				iosym.pop();
				cmds.pop();
			}
			else if(iosym.front() == "|") {
				pipeio = true;
				piping fd;
				pipelist.push_back(fd);
				if(-1 == pipe(pipelist.at(pipecnt).fd)) {
					perror("There was an error with pipe().");
					exit(1);
				}
				iosym.pop();
			}
		}
		//WHERE WORK IS DONE
		if(!endcmd) {
		int pidio = fork();

		if(-1 == pidio) {
			perror("Error with fork().");
			exit(1);
		}
		if(0 == pidio) {		//Child
			if(leftio && -1 == close(0)) {
				perror("Error with close(0).");
				exit(1);
			}
			if((rightio || rightio2) && -1 == close(1)) {
				perror("Error with close(1).");
				exit(1);
			}
			const char* ifile = infiles.c_str();
			const char* ofile = outfiles.c_str();
			int flag1 = O_WRONLY|O_CREAT|O_TRUNC;
			const char* ofile2 = outfiles2.c_str();
			int flag2 = O_WRONLY|O_CREAT|O_APPEND;
			int perm = S_IRUSR|S_IWUSR;
			int fdi;
			if(leftio && -1 == (fdi = open(ifile, O_RDONLY))) {
				perror("Error with open(). (<)");
				exit(1);
			}
			int fdo;
			if(rightio && -1 == (fdo = open(ofile, flag1, perm))) {
				perror("Error with open(). (>)");
				exit(1);
			}
			if(rightio2 && -1 == (fdo = open(ofile2, flag2, perm))) {
				perror("Error with open(). (>>)");
				exit(1);
			}
			if(pipeio) {
				if(-1 == dup2(pipelist.at(pipecnt).fd[PIPE_WRITE], 1)) {
					perror("Error with dup2(). ");
					exit(1);
				}
				pipeio = false;
			}
			if(completepipe) {
				if(-1 == dup2(pipelist.at(pipecnt-1).fd[PIPE_READ], 0)) {
					perror("Error with dup2().");
					exit(1);
				}
			}
			if(-1 == execvp(cmd[0], cmd)) {
				perror("Error with execvp().");
				exit(1);
			}
		}

		else if(0 < pidio) {		//Parent
			if(!pipeio && -1 == waitpid(-1, 0, 0)) {
				perror("Error with wait().");
				exit(1);
			}
			if(leftio3) {
				if(0 != remove(tmpfile)) {
					perror("Error with remove.");
					exit(1);
				}
			}
			if(completepipe) {		//Closes pipe
				if(-1 == close(pipelist.at(pipecnt-1).fd[PIPE_READ])) {
					perror("Error with close.");
					exit(1);
				}
				completepipe = false;
			}
			if(pipeio){
				if(-1 == close(pipelist.at(pipecnt).fd[PIPE_WRITE])) {
					perror("Error with close().");
					exit(1);
				}

				pids.emplace_back(pidio);
				pipecnt++;
				forkcnt++;
				completepipe = true;
			}
		}
		}

		}
	}
		
	return endprog;
}

int findstr(string param, string &newstr) {
	string out = param;
	int first = out.find("\"");
	if(-1 == first) {
		newstr = out;
		return -1;
	}
	int second = out.find("\"", first+1);
	if(-1 == second) {
		newstr = out;
		return 1;
	}
	else {
		newstr = out.substr(first+1, second-first-1) + "\n";
		return 0;
	}
}
