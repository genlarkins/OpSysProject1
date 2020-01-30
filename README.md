# OpSysProject1

Problems with executing "normal" commands:
	-Executing a non-existing command followed by existing commands will cause those commands to not execute. 	This is due to a problem with    altering the $PATH file.
  
Problems with executing built-in commands:
	-cd does not give error if two arguments are given
	-cd followed by no argument will not go to home directory, user will just stay in current directory.
	-cd does not take into account "." and ".."
