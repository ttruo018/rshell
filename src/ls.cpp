#include <iostream>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <iomanip>
#include <algorithm>
#include <string>
#include <cctype>

using namespace std;

void separatearg(vector<char* > &flags, vector<char* > &dirfiles, int argc, char** argv);

void printarg(vector<char* > flags, vector<char* > dirfiles);	//FOR error checking


int checkflags(vector<char* > flags);

int flagset(char flag);

void opendirfile(char* dirfile, int flags);

int main(int argc, char** argv) {
	if(argc<=1) {		//if true, then no flags passed in
		cout << "No flags" << endl;		//TEMPORARY
		//RUN ls on . directory
		char currentdir[] = ".";
		opendirfile(currentdir, 0);
	}
	else {
		vector<char* > flagsvec;
		vector<char* > dirfiles;
		separatearg(flagsvec, dirfiles, argc, argv);
		//printarg(flags,dirfiles);
		if(dirfiles.empty()) {
			//THEN check flags -> run ls on . directory
			int flags = checkflags(flagsvec);
			char currentdir[] = ".";
			opendirfile(currentdir, flags);

		}
		else {
			//THEN check flags -> run ls on files stated
			int flags = checkflags(flagsvec);
			for(unsigned int i=0; i<dirfiles.size(); ++i) {
				opendirfile(dirfiles[i], flags);
			}
		}
		
	}

	return 0;
}

void separatearg(vector<char* > &flags, vector<char* > &dirfiles, int argc, char** argv) {
	for(int i=1; i<argc; ++i) {
		cout << "The arguement is " << argv[i] << endl;
		if(argv[i][0]=='-') {
			flags.push_back(argv[i]);
		}
		else {
			dirfiles.push_back(argv[i]);
		}
	}
}

void printarg(vector<char* > flags, vector<char* > dirfiles) {
	cout << "flags: ";
		for(unsigned int i=0; i<flags.size(); ++i) {
			cout << flags[i] << " ";
		}
		cout << endl;

		cout << "dirfiles: ";
		for(unsigned int i=0; i<dirfiles.size(); ++i) {
			cout << dirfiles[i] << " ";
		}
		cout << endl;
}

int checkflags(vector<char* > flags) {
	int out = 0;
	if(flags.empty()) {
		return out;
	}
	else{
		for(unsigned int i=0; i<flags.size(); ++i) {
			for(unsigned int j=1; flags[i][j]!=0; ++j) {
				out = out | flagset(flags[i][j]);
			}
		}
	}
	cout << "flags: " << out << endl;
	return out;
}

int flagset(char flag) {
	int out = 0;
	if(flag=='a') {
		out = out | 01;
	}
	else if(flag=='l') {
		out = out | 02;
	}
	else if(flag=='R') {
		out = out | 04;
	}
	else {
		cout << "Invalid option -- '" << flag << "'" << endl;
	}
	return out;
}

//bool cstrcomp(const char* a, const char* b) {
//	char const** char_a = a;
//	char const** char_b = b;
	//return ((strcmp(a, b)<0) ? false : true);
//}

bool stringcomp(string a, string b) {
	for(unsigned int i=0; i<a.size() && i<b.size(); ++i) {
		if(tolower(b[i]) < tolower(a[i])) {
			return false;
		}
		else if(tolower(a[i]) < tolower(b[i])) {
			return true;
		}
	}
	if(b.size() < a.size()) {
		return false;
	}
	else {
		return true;
	}
}

void opendirfile(char* dirfile, int flags) {
	DIR* dirp;
	if(NULL == (dirp = opendir(dirfile))) {
		perror("Error with opendir(). ");
		exit(1);
	}
	struct dirent *filespecs;
	vector<string> list;
	unsigned char maxlen = 0;
	errno = 0;
	while(NULL != (filespecs = readdir(dirp))) {
		if((filespecs->d_name[0]!='.') || (flags & 01)) {	//checks flag a
			list.push_back(filespecs->d_name);
			if(maxlen < strlen(filespecs->d_name)) {
				maxlen = strlen(filespecs->d_name);
			}
		}
	}
	if(errno != 0) {
		perror("There was an error with readdir(). ");
		exit(1);
	}
	maxlen += 3;
	sort(list.begin(), list.end(), stringcomp);
	unsigned int linewidth = 0;
	cout << left;
	if(flags & 02) {	//checks -l flag
		//OUTPUT WITH -l STYLE
	}
	else {
		for(unsigned int i=0; i<list.size(); ++i) {
			linewidth += maxlen;
			if(linewidth > 80) {
				cout << "\n";
				linewidth = 0;
			}
			cout << setw(maxlen) << list[i];
		}
		cout << endl;
	}
	if(flags & 04) {	//checks -R flag
		//DO -R THINGS
		for(unsigned int i=0; i<list.size(); ++i) {
			if(list[i]!="." && list[i]!="..") {
				if(S_ISDIR) {
					opendir(list[i]);
				}
			}
	}
	if(-1 == closedir(dirp)) {
		perror("There was an error with closedir(). ");
		exit(1);
	}
}
