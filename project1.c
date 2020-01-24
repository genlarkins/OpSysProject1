#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

char* prompt();

int main(){
	
	//prompt user
	char* userInput= "nothing";
	int counter=0;
	while(strcmp(userInput, "exit") != 0){
		userInput = prompt();
		printf("Got the input: %s\n", userInput);
		//parse input
		char* test= malloc(sizeof(char));
		//execute counter++ for every command executed
		
	}
	printf("Exiting now!\n");
	return 0;
}

char* prompt(){
	char* input = malloc(sizeof(char));
	char* user = getenv("USER");
	char* machine = getenv("MACHINE");
	char* pwd = getenv("PWD");
	printf("%s@%s: %s>", user, machine, pwd);
	//get the command a user inputs
	fgets(input, 50, stdin);
	strtok(input, "\n");
	return input;
}