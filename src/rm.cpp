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

void recursive_delete(string directory)
{       
    DIR* dirp;
    if(NULL == (dirp = opendir(directory.c_str())))
    {
        perror("opendir");
        exit(1);
    }
    struct dirent *filespecs;
    while(NULL != (filespecs = readdir(dirp)))
    {
        const char* file_name = filespecs->d_name;
        struct stat statbuf;

        string file_hold = file_name;
        file_hold = directory + "/" + file_hold;

        if(-1 == stat(file_hold.c_str(), &statbuf))
        {
            perror("stat");
            exit(1);
        }      
        if(S_ISREG(statbuf.st_mode))
        {
            if(-1 == unlink(file_hold.c_str()))
            {
                perror("unlink");
                exit(1);
            }
        }
        
        else if(S_ISDIR(statbuf.st_mode))
        {
            if(file_name[0] != '.'){
                if(-1 == rmdir(file_hold.c_str())){
                    recursive_delete(file_hold);
                }
            }
        }
    }
    if(-1 == closedir(dirp))
    {
        perror("closedir");
        exit(1);
    }
    rmdir(directory.c_str());
}

int main(int argc, char** argv)
{
    bool r_flag = false;
    for(int i = 1; i < argc; i++)
    {
        char flagchar[] = "-r";
        if(strcmp(flagchar,argv[i]) == 0)
        {
            r_flag = true;
        }
    }

    for(int i = 1; i < argc; i++)
    {
        char flagchar[] = "-r";
        if(strcmp(flagchar,argv[i]) != 0)
        {
        struct stat statbuf;
        const char* file_name = argv[i];
        if(-1 == stat(file_name, &statbuf))
        {
            perror("stat");
            exit(1);
        }

        if(S_ISREG(statbuf.st_mode))
        {
            if(-1 == unlink(file_name))
            {
                perror("unlink");
                exit(1);
            }
        }

        else if(S_ISDIR(statbuf.st_mode))
        {
            if(-1 == rmdir(file_name)){
                if(r_flag)
                {
                    string directory = "./";
                    directory += file_name;
                    recursive_delete(directory);
                }
                else
                {
                    perror("rmdir");
                    exit(1);
                }
            }
        }
        }
    }

    return 0;
}
