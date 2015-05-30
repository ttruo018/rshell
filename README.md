# Rshell
## Description
Rshell can execute terminal commands, similar to the bash shell. 
Rshell is created for UCR CS100 class Spring 2015.
The shell can run simple commands such as `ls` and `pwd`, as well as the commands' flags. 
Commands and connectors are read left to right.   
You can also change your current working directory with the `cd` command.   
The command `cd` will take you to your home directory.   
The command `cd -` will take you to the last directory you were in.   
The command `cd PATH` will take you to the directory described by PATH.   

###Connectors
Connectors such as `;`, `||`, and `&&` can be used in between commands on a single line.
Some of the connectors' behaviors are:     
* Connectors with more than two characters, ex `&&&&&&` or `||||`,
will be treated the same as two character connectors `&&` and `||`.     
* Currently, there is a bug when only one character, like `&` and `|`
is inputted as a connector. It is currently being treated the same as a `;` connector.     
* If a connector is on the beginning or end by itself, the connector will be ignored.     
* If multiple connectors are next to each other, only the first one is considered.

###I/O Redirection and Piping
The shell can redirect files or a string as inputs 
with `<` and `<<<`, redirect output to files 
with `>` and `>>`, and pipe commands together with `|`. 
* With the `<` symbol, you can redirect a file as input to 
a command. If more than one file is provided, only the very 
last file given will be used as input.    
* With the `<<<` symbol, you can redirect a string as 
the input to a command. The string must be enclosed within double quotation marks (ie `"This is a string"`), otherwise an
error message will be shown.       
* With the `>` symbol, you can redirect the command's output 
into a file, creating the file if it doesn't already exist. 
If the file already exists, then the file's contents will be 
erased and the command's output will replace it.
* With the `>>` symbol, you can also redirect a command's 
output into a file. However if the file exists, the 
command's output will be appended onto the existing file, 
rather than deleting its contents.   
* With the `|` symbol, the output of the file on the left of 
this symbol will be the input of the command on the right.   

###Signals   
While in Rshell, pressing Control-C will end any child process that is running 
and return to the Rshell. 
For example, if you run `$ cat`, the `cat` command will continuously wait for stdin. If you press 
Control-C during the process, the cat command will end and Rshell will display the prompt. 
This also applies to subsequent Rshell programs, meaning only the first Rshell will remain 
if Control-C is pressed.   

###Exit command
Typing an `exit` command will exit Rshell if run. 
For example, `ls || exit` will only perform the `ls` command.
But `ls && exit` will perform the `ls` command first, then exit Rshell.

##ls   
This `ls` command runs similar to the GNU `ls` command, 
but only being able to use the flags `-a`, `-l`, and `R`. 
Any combination of these flags as well as any 
file or directory can be passed along with the `ls`. 
The `-a` command makes `ls` print hidden files. 
The `-l` command makes `ls` print info about each file or 
directory. The `R` flag make `ls` run recursively. 

##Building and Running
To download and build:
	$ git clone https://github.com/ttruo018/rshell   
	$ cd rshell   
	$ git checkout hw3   
	$ make

To run Rshell, use the following command:
`$ bin/rshell`   

To run ls, use the following commands:    
`$ bin/ls`   

All binary executables will be in the bin directory of the rshell directory.

##Known Bugs and Issues
###rshell
* A single `|` or `&` will let both commands run. 
It performs the same as the `;` connector.   
* Empty arguements may have unexpected behavior    
* Running Rshell in Rshell will only keep track of the newest Rshell.
Run the `ps` command to check how much Rshells are running.   
* Occasionally, the prompt will output before the command's 
output. Just press the ENTER key to get the prompt back.   
* The `<<<` operator seems to not work in scripts.   
* When combining connectors and I/O redirection(or piping), 
there may be undefined behavior.   
* Can only redirect each input and output to one file.   
* Pressing Control-C will take you back to the first Rshell, 
even if you are running an Rshell within an Rshell

###ls
* running `-l` and `-R` in any combination on a folder 
containing many directories may cause errors with 
getpwuid() and getgrgid(), can become
common if using `-a`, `-l` and `-R` flags together    
* running `-l` may cause the error `do_ypcall: clnt_call: 
RPC: Unable to send; errno = Operation not permitted`    
* The number of links on the `.` directories can be incorrect.    
* The time on the `..` directories can be incorrect.     
* When using `-l` on symbolic links, the output does not show 
what the file is linked to, unlike the GNU `ls`.   

