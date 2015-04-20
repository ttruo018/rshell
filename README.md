# Rshell
## Description
Rshell can execute terminal commands, similar to the bash shell. 
Rshell is created for UCR CS100 class Spring 2015.
The shell can run simple commands such as `ls` and `pwd`, as well as the commands' flags. 
But unlike the bash shell, it cannot run `cd`. 

Connectors such as `;`, `||`, and `&&` can be used in between commands on a single line.
Some of the connectors' behaviors are:     
*Connectors with more than two characters, ex `&&&&&&` or `||||`,
will be treated the same as two character connectors `&&` and `||`.     
*Currently, there is a bug when only one character, like `&` and `|`
is inputted as a connector. It is currently being treated the same as a `;` connector.    

Typing an `exit` command will exit Rshell if run. 
For example, `ls || exit` will only perform the `ls` command.
But `ls && exit` will perform the `ls` command first, then exit Rshell.

##Building and Running
To download, build, and run Rshell, use the following commands:

`$ git clone https://github.com/ttruo018/rshell`   
`$ cd rshell`   
`$ git checkout hw0`   
`$ make`   
`$ bin/rshell`   

The rshell binary executable will be in the bin directory of the rshell directory.

##Known Bugs and Issues
*A single `|` or `&` will let both commands run. 
It performs the same as the `;` connector.   
*Empty arguements may have unexpected behavior    
*Having multiple connectors next to each other has unwanted behavior   
*Running Rshell in Rshell will only keep track of the newest Rshell.
Run the `ps` command to check how much Rshells are running.   


