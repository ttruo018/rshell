# Rshell
Rshell is a shell created to be a simpler version of the bash shell. 
Rshell is created for UCR CS100 class Spring 2015.
The shell can run simple commands such as `ls` and `pwd`, as well as the commands' flags. 
But unlike the bash shell, it cannot run `cd`. 
It can also use the `;`, `||`, and `&&` connectors (however, there may be bugs if there are two connectors next to each other)


#Building and Running
To download, build, and run Rshell, use the following commands:

`$ git clone https://github.com/ttruo018/rshell`

`$ cd rshell`

`$ git checkout hw0`

`$ make`

`$ bin/rshell`

#Known Bugs
*A single `|` or `&` will let both commands run. 
It performs the same as the `;` connector.

*Did not handle empty commands yet

*Having two connectors next to each other has unwanted behavior

