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
#include <time.h>
#include <pwd.h>
#include <grp.h>

using namespace std;

void separatearg(vector<char* > &flags, vector<char* > &dirfiles, int argc, char** argv);

void printarg(vector<char* > flags, vector<char* > dirfiles);	//FOR error checking


int checkflags(vector<char* > flags);

int flagset(char flag);

void opendirfile(char* dirfile, int flags);

void singlefile(char* file, int flags);

void loutput(char* file, vector<string> &perm, vector<string> &link, vector<string> &usrid, 
				vector<string> &grpid, vector<string> &fsize, vector<string> &date, 
				vector<string> &fname);

unsigned int largvecelem(vector<string> v);

int main(int argc, char** argv) {
	if(argc<=1) {		//if true, then no flags passed in
		//RUN ls on . directory
		char currentdir[] = ".";
		opendirfile(currentdir, 0);
	}
	else {
		vector<char* > flagsvec;
		vector<char* > dirfiles;
		separatearg(flagsvec, dirfiles, argc, argv);
		//printarg(flags,dirfiles);
		int flags = checkflags(flagsvec);
		if(dirfiles.empty()) {
			//THEN check flags -> run ls on . directory
			char currentdir[] = ".";
			opendirfile(currentdir, flags);
		}
		else {
			//THEN check flags -> run ls on files stated
			for(unsigned int i=0; i<dirfiles.size(); ++i) {
				struct stat fd;
				if(-1 == (stat(dirfiles[i], &fd))) {
					cout << "Error with stat() called on " << dirfiles[i]
						<< endl;
				}
				else if(S_ISDIR(fd.st_mode)) {
					opendirfile(dirfiles[i], flags);
				}
				else {
					singlefile(dirfiles[i], flags);
				}

			}
		}
		
	}

	return 0;
}

void separatearg(vector<char* > &flags, vector<char* > &dirfiles, int argc, char** argv) {
	for(int i=1; i<argc; ++i) {
		//cout << "The arguement is " << argv[i] << endl;
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
	//cout << "flags: " << out << endl;
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
	unsigned int totalsize = 0;
	errno = 0;
	while(NULL != (filespecs = readdir(dirp))) {
		if((filespecs->d_name[0]!='.') || (flags & 01)) {	//checks flag a
			list.push_back(filespecs->d_name);
			totalsize += strlen(filespecs->d_name);
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
	if(flags & 02) {
		unsigned int blksize = 0;
		for(unsigned int i=0; i<list.size(); ++i) {
			struct stat fd;
			if(-1 == (stat(dirfile,&fd))) {
				cout << "Error with calling stat on " << dirfile << endl;
			}
			else {
				blksize += (fd.st_blocks/2);
			}
		}
		cout << "total " << blksize << endl;
	}
	if(flags & 06) {
		for(unsigned int i=0; i< list.size(); ++i) {
			if(list[i]!="." && list[i]!="..") {
				string strdirfile = dirfile;
				list[i] = strdirfile + "/" + list[i];
			}
		}
	}
	unsigned int linewidth = 0;
	cout << left;
	if(flags & 04) {
			cout << dirfile << ":" << endl;
	}
	if(flags & 02) {	//checks -l flag
		//unsigned int outlen = 0;
		vector<string> perm;
		vector<string> link;
		vector<string> usrid;
		vector<string> grpid;
		vector<string> fsize;
		vector<string> date;
		vector<string> fname;
		for(unsigned int i=0; i<list.size(); ++i) {
			loutput(const_cast<char* >(list[i].c_str()), perm, link, usrid, grpid, 
					fsize, date, fname);
		}
		unsigned int permlen = largvecelem(perm);
		unsigned int linklen = largvecelem(link);
		unsigned int usridlen = largvecelem(usrid);
		unsigned int grpidlen = largvecelem(grpid);
		unsigned int fsizelen = largvecelem(fsize);
		unsigned int datelen = largvecelem(date);
		unsigned int fnamelen = largvecelem(fname);
		for(unsigned int i=0; i<fname.size(); ++i) {
			cout << right;
			cout << setw(permlen) << perm[i] << ' ';
			cout << setw(linklen) << link[i] << ' ';
			cout << setw(usridlen) << usrid[i] << ' ';
			cout << setw(grpidlen) << grpid[i] << ' ';
			cout << setw(fsizelen) << fsize[i] << ' ';
			cout << setw(datelen) << date[i] << ' ';
			cout << left;
			cout << setw(fnamelen) << fname[i] << ' ';
			cout << endl;
		}
	}
	else {
		for(unsigned int i=0; i<list.size(); ++i) {
			if(totalsize < 70) {
				cout << list[i] << "  ";
			}
			else {
				linewidth += maxlen;
				if(linewidth > 80) {
					cout << "\n";
					linewidth = 0;
				}
				cout << setw(maxlen) << list[i];
			}
		}
		cout <<  endl;
	}
	if(flags & 04) {	//checks -R flag
		for(unsigned int i=0; i<list.size(); ++i) {
			if(list[i]!="." && list[i]!="..") {
				struct stat fd;
				if(-1 == (stat(list[i].c_str(), &fd))) {
					cout << "Error on stat calling " << list[i]
						<< endl;
				}
				else if(S_ISDIR(fd.st_mode)) {
					cout << endl;
					opendirfile(const_cast<char* >(list[i].c_str()),flags);
				}
			}
		}
	}
	if(-1 == closedir(dirp)) {
		perror("There was an error with closedir(). ");
		exit(1);
	}
}

void singlefile(char* file, int flags) {
	vector<string> perm;
	vector<string> link;
	vector<string> usrid;
	vector<string> grpid;
	vector<string> fsize;
	vector<string> date;
	vector<string> fname;
	if(flags & 02) {	//checks -l flag
		loutput(file, perm, link, usrid, grpid, fsize, date, fname);
		cout << file << endl;
	}
	else {
		cout << file << endl;
	}
}

void loutput(char* file, vector<string> &perm, vector<string> &link, vector<string> &usrid, 
				vector<string> &grpid, vector<string> &fsize, vector<string> &date,
				vector<string> &fname) {
	struct stat fd;
	if(-1 == (stat(file, &fd))) {
		cout << "Error with stat calling " << file << endl;
	}
	if(S_ISREG(fd.st_mode)) {
//		cout << '-';
		perm.push_back("-");
	}
	else if(S_ISDIR(fd.st_mode)) {
//		cout << 'd';
		perm.push_back("d");
	}
	else if(S_ISCHR(fd.st_mode)) {
//		cout << 'c';
		perm.push_back("c");
	}
	else if(S_ISBLK(fd.st_mode)) {
//		cout << 'b';
		perm.push_back("b");
	}
	else if(S_ISLNK(fd.st_mode)) {
//		cout << 'l';
		perm.push_back("l");
	}
//	(fd.st_mode & S_IRUSR) ? (cout << 'r') : (cout << '-');
//	(fd.st_mode & S_IWUSR) ? (cout << 'w') : (cout << '-');
//	(fd.st_mode & S_IXUSR) ? (cout << 'x') : (cout << '-');
//	(fd.st_mode & S_IRGRP) ? (cout << 'r') : (cout << '-');
//	(fd.st_mode & S_IWGRP) ? (cout << 'w') : (cout << '-');
//	(fd.st_mode & S_IXGRP) ? (cout << 'x') : (cout << '-');
//	(fd.st_mode & S_IROTH) ? (cout << 'r') : (cout << '-');
//	(fd.st_mode & S_IWOTH) ? (cout << 'w') : (cout << '-');
//	(fd.st_mode & S_IXOTH) ? (cout << 'x') : (cout << '-');
(fd.st_mode&S_IRUSR)?perm[perm.size()-1]=(perm.back()+"r"):perm[perm.size()-1]=(perm.back()+"-");
(fd.st_mode&S_IWUSR)?perm[perm.size()-1]=(perm.back()+"w"):perm[perm.size()-1]=(perm.back()+"-");
(fd.st_mode&S_IXUSR)?perm[perm.size()-1]=(perm.back()+"x"):perm[perm.size()-1]=(perm.back()+"-");
(fd.st_mode&S_IRGRP)?perm[perm.size()-1]=(perm.back()+"r"):perm[perm.size()-1]=(perm.back()+"-");
(fd.st_mode&S_IWGRP)?perm[perm.size()-1]=(perm.back()+"w"):perm[perm.size()-1]=(perm.back()+"-");
(fd.st_mode&S_IXGRP)?perm[perm.size()-1]=(perm.back()+"x"):perm[perm.size()-1]=(perm.back()+"-");
(fd.st_mode&S_IROTH)?perm[perm.size()-1]=(perm.back()+"r"):perm[perm.size()-1]=(perm.back()+"-");
(fd.st_mode&S_IWOTH)?perm[perm.size()-1]=(perm.back()+"w"):perm[perm.size()-1]=(perm.back()+"-");
(fd.st_mode&S_IXOTH)?perm[perm.size()-1]=(perm.back()+"x"):perm[perm.size()-1]=(perm.back()+"-");

//	cout << ' ';
//	cout << fd.st_nlink << ' ';
	char numlink[20];
	int nlink = fd.st_nlink;
	sprintf(numlink, "%d", nlink);
	link.push_back(numlink);
	struct passwd *usr;
	usr = getpwuid(fd.st_uid);
//	cout << usr->pw_name << ' ';
	usrid.push_back(usr->pw_name);
	struct group *grp;
	grp = getgrgid(fd.st_gid);
//	cout << grp->gr_name << ' ';
	grpid.push_back(grp->gr_name);
//	cout << fd.st_size << ' ';
	char numfsize[20];
	int nfsize = fd.st_size;
	sprintf(numfsize, "%d" , nfsize);
	fsize.push_back(numfsize);
	time_t now;
	time(&now);
	struct tm* currtime;
	currtime = localtime(&now);
	struct tm* filetime;
	int curryear = currtime->tm_year;
	filetime = localtime(&fd.st_ctime);
	char buf[30];
	if(curryear != filetime->tm_year) {
		strftime(buf, 30, "%b %e %Y", filetime);
	}
	else {
		strftime(buf, 30, "%b %e %H:%M", filetime);
	}
//	cout << buf << ' ';
	date.push_back(buf);
	string strfile = file;
	int fileloc= strfile.find_last_of("/");
//	(fileloc==-1) ? cout << strfile : cout << strfile.substr(fileloc+1);
	(fileloc==-1) ? fname.push_back(strfile) : fname.push_back(strfile.substr(fileloc+1));

//	cout << endl;
}

unsigned int largvecelem(vector<string> v) {
	unsigned int out = 0;
	for(unsigned int i=0; i<v.size(); ++i) {
		if(out < v[i].size()) {
			out = v[i].size();
		}
	}
	return out;
}

