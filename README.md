# Rshell
## Description
Rshell is a shell created to be a simpler version of the bash shell. 
Rshell is created for UCR CS100 class Spring 2015.
The shell can run simple commands such as `ls` and `pwd`, as well as the commands' flags. 
But unlike the bash shell, it cannot run `cd`. 

It can also use the `;`, `||`, and `&&` connectors in between commands on a single line
(however, there may be bugs if there are two connectors next to each other).
If the user inputs a connector with more than two characters, ex `&&&&&&` or `||||`, 
it will be treated the same as if the normal connectors `&&` and `||`. 
Currently, there is a bug when only one character, like `&` and `|`
is inputted as a connector. It is currently being treated the same as a `;` connector.

The `exit` command will exit Rshell if run. 
For example, `ls || exit` will only perform the `ls` command.
But `ls && exit` will perform the `ls` command first, then exit Rshell.

##Building and Running
To download, build, and run Rshell, use the following commands:

`$ git clone https://github.com/ttruo018/rshell`   
`$ cd rshell`   
`$ git checkout hw0`   
`$ make`   
`$ bin/rshell`   

##Known Bugs and Issues
*A single `|` or `&` will let both commands run. 
It performs the same as the `;` connector.   
*Did not handle empty commands yet   
*Having two connectors next to each other has unwanted behavior
*Running Rshell in Rshell will only keep track of the newest Rshell.
Run the `ps` command to check how much Rshells are running.   


