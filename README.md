# OpSysProject1
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
