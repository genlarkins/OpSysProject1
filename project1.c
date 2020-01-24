#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

char* prompt();

//function to check existence of file
bool exists(const char *fname);

//function to check if file is regular
bool check_regular(const char *fname);

//function to execute commands
void my_execute(char **cmd); 

int main(){
	
	//prompt user
	int size = 4;
	char *userInput = (char *)malloc(sizeof(char)*size);
	char * token = strtok(getenv("PATH"), ":");
	//string for concatonated path names
	char * concat = (char *)malloc(sizeof(char)*size);
	//string containing command + args
	char ** cmd = (char **)malloc(sizeof(char)*size);
	int counter=0;
	while(strcmp(userInput, "exit") != 0){
		strcpy(userInput, "");
		userInput = prompt();
		printf("Got the input: %s\n", userInput);
		//parse input
		char* test= malloc(sizeof(char));
		//execute counter++ for every command executed
		
		//loop through to get all paths
		//if flag is still false, file is not executable
		bool file_flag = false;
		while(token != NULL){
			//printf( " %s\n", token );
			//path concatenated with provided pathname
			strcpy(concat, token);
			strcat(concat, "/");
			strcat(concat, userInput);
			//printf(" %s\n", concat);
			if(exists(concat) && check_regular(concat)){
				//strcpy(concat, "");
				file_flag = true;
				break;
			}  
			strcpy(concat, "");
			token = strtok(NULL, ":");
		} 

    if(!file_flag)
      printf("%s\n", "File does not exist or is not regular");
    else {
      cmd[0] = concat;
      my_execute(cmd);
    }
    //strcpy(userInput, "");
}
return 0;		
}
char* prompt(){
	char* input = malloc(sizeof(char));
	char* user = getenv("USER");
	char* machine = getenv("MACHINE");
	char* pwd = getenv("PWD");
	printf("%s@%s: %s>", user, machine, pwd);
	//get the command a user inputs
	scanf("%s", input);
	//fgets(input, 50, stdin);
	//strtok(input, "\n");
	return input;
}

bool exists(const char *fname){
  if(access(fname, F_OK) != -1){
    //printf("%s\n", "file exists");
    return true;
  }
  else {
    return false;
  }
}

bool check_regular(const char *fname){
  struct stat file_stat;
  stat(fname, &file_stat);
  return S_ISREG(file_stat.st_mode);
}

void my_execute(char **cmd) {
  int status;
  pid_t pid = fork();
  if (pid == -1) {
    //Error
    exit(1);
  }
  else if (pid == 0) {
    //Child
    execv(cmd[0], cmd);
    printf("Problem executing %s\n", cmd[0]);
    exit(1);
  }
  else {
    //Parent
    waitpid(pid, &status, 0);
  }
}
