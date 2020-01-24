/*Madison Vandersluis
 *This will contain solutions for project 1
 *Specifically: prompt, I/O redirection, Background Processing
 *Due: Jan 28, 2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

char* prompt();
void bgProcess(char* data);
void checkBGP();
void ioRedirection(char* cmd, char* file, int i_o);
void ioRedirection(char* cmd, char* file_in, char* file_out);

int main(){
	
	//prompt user
	char* userInput= "nothing";
	while(strcmp(userinput, "exit(1)") != 0){
		userInput = prompt();
		printf("Got the input: %s", userinput);
		checkBGP();
		//parse input
	}
	
	return 0;
}

char* prompt(){
	char* input;
	char* user = getenv("USER");
	char* machine = getenv("MACHINE");
	char* pwd = getenv("PWD");
	printf("%s@%s: %s >", user, machine, pwd);
	//get the command a user inputs
	fgets(input, 50, stdin);
	return input;
}

void bgProcess(char* data){}
void checkBGP(){}
void ioRedirection(char* cmd, char* file, int i_o){
	int fd, buff;
	char* fileData;
	if(i_o==0){ //output
		//redirect cmd result to file
	}
	else if(i_o==1){ //input
		//check if file exists
		//error if it doesnt else
		//execute cmd with file as input
		fd = open(file);
		if(fd == -1){ //means error opening file
			printf("Error opening file: %s", file);
		}
		else{
			if(forked() == 0){
				//child
				close(STDIN_FILENO);
				dup(fd);
				close(fd);
				//execute process bscially it will read in from the file which is now "stdin"
				//call execution using fileData as input
			}
			else {
				//parent
				close(fd);
			}
		}
	} else{
		//error
	}
	
}
void ioRedirection(char* cmd, char* file_in, char* file_out){
	//check if file_in exists error if it dont
	//cmd takes filein as input and saves result in fileout
}