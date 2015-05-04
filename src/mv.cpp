#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

int main(int argc, char** argv)
{
    struct stat statbuf;
    if(-1 == stat(argv[2], &statbuf))
    {
        perror("stat");
        exit(1);
    }
    if(S_ISDIR(statbuf.st_mode))
    {
        string directory = argv[2];
        string file_name = argv[1];
        string new_file = directory + "/" + file_name;
        if( -1 == link(argv[1], new_file.c_str()))
        {
            perror("link");
            exit(1);
        }

        if(-1 == unlink(argv[1]))
        {
            perror("unlink");
            exit(1);
        }
    }
    else
    {
        if( -1 == link(argv[1], argv[2]))
        {
            perror("link");
            exit(1);
        }

        if(-1 == unlink(argv[1]))
        {
            perror("unlink");
            exit(1);
        }
    }

}
