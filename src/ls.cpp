#include <iostream>
#include <vector>

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

void opendirfile(char* dirfile, int flags) {

}
