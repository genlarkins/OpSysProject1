#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void prompt(char* input);

int main(){
	
	//prompt user
	char* userInput= "nothing";
	int counter=0;
	while(strcmp(userInput, "exit") != 0){
		if(strcmp(userInput,"") != 0){
			printf("Got the input: %s\n", userInput);
			//parse input
			//execute counter++ for every command executed
		}
	}
	free(userInput);
	printf("Exiting now!\n");
	return 0;
}

void prompt(char* input){
	char* user = getenv("USER");
	char* machine = getenv("MACHINE");
	char* pwd = getenv("PWD");
	printf("%s@%s: %s>", user, machine, pwd);
	//get the command a user inputs
	fgets(input, 50, stdin);
	strtok(input, "\n");
}