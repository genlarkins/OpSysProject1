# OpSysProject1
Team Members: Madison Vandersluis, Jared Rice, and Genevieve Larkins
Division of Labor: 
  Madison Vandersluis: prompt, i/o redirection, background processes
  Jared Rice: $PATH resolution, execution, built-ins
  Genevieve Larkins: environmental variables, shortcut resolution, pipes
  
Contents of Tar Archive:
  Makefile
  Project1.c
  README
  
How to Compile Executables Using Makefile:
  We only have one file and we execute that by using gcc -o shell proj1.c


Prompt works as expected.
Exit leaves function.

Problems with executing "normal" commands:

	-Executing a non-existing command followed by existing commands will cause those commands to not execute. This is due to a problem with altering the $PATH file.
  
Problems with executing built-in commands:

	-cd does not give error if two arguments are given
	-cd followed by no argument will not go to home directory, user will just stay in current directory.
	-cd does not take into account "." and ".."
I/O redirection:
	Invalid format signals error
	Works for CMD < FILE
	Sometimes works for CMD < FILE > FILE and CMD > FILE < FILE
	Known issues: CMD > FILE sometimes causes memory leaks. CMD < FILE > FILE and CMD > FILE < FILE don’t run due to permission errors or memory leaks
BGProcess:
Invalid format signals error
Issues: Partially works for CMD < FILE &. But the result of the command prints after
Not implemented for normal execution or piping

Pipe segfaults
