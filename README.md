# Rshell
## Description
Rshell can execute terminal commands, similar to the bash shell. 
Rshell is created for UCR CS100 class Spring 2015.
The shell can run simple commands such as `ls` and `pwd`, as well as the commands' flags. 
But unlike the bash shell, it cannot run `cd`.   
Commands and connectors are read left to right.   

###Connectors
Connectors such as `;`, `||`, and `&&` can be used in between commands on a single line.
Some of the connectors behaviors are:     
*Connectors with more than two characters, ex `&&&&&&` or `||||`,
will be treated the same as two character connectors `&&` and `||`.     
*Currently, there is a bug when only one character, like `&` and `|`
is inputted as a connector. It is currently being treated the same as a `;` connector.     
*If a connector is on the beginning or end by itself, the connector will be ignored.     
*If multiple connectors are next to each other, only the first one is considered.

###Exit command
Typing an `exit` command will exit Rshell if run. 
For example, `ls || exit` will only perform the `ls` command.
But `ls && exit` will perform the `ls` command first, then exit Rshell.

##Building and Running
To download:
`$ git clone https://github.com/ttruo018/rshell`   
`$ cd rshell`   

To build, and run Rshell, use the following commands:

`$ git checkout hw0`   
`$ make`   
`$ bin/rshell`   

To build, and run ls, use the following commands:
`$ git checkout hw1`   
`$ make`   
`$ bin/ls`   

The all binary executables will be in the bin directory of the rshell directory.

##Known Bugs and Issues
###rshell
*A single `|` or `&` will let both commands run. 
It performs the same as the `;` connector.   
*Empty arguements may have unexpected behavior    
*Running Rshell in Rshell will only keep track of the newest Rshell.
Run the `ps` command to check how much Rshells are running.   
###ls
*running -l and -R in any combination on a folder 
containing many directories may cause errors with 
getpwuid() and getgrgid(), can become
common if using -a, -l and -R flags together    
*running -l may cause the error `do_ypcall: clnt_call: 
RPC: Unable to send; errno = Operation not permitted`    
*The number of links on the . directories and time on the 
.. directories can be incorrect.     
*When using -l on symbolic links, the output does not show 
what the file is linked to, unlike the GNU ls.   

