/*Madison Vandersluis
 *This will contain solutions for project 1
 *Specifically: prompt, I/O redirection, Background Processing
 *Due: Jan 28, 2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void prompt(char* input);
void bgProcess(char* data);
void checkBGP(int counter);
void ioRedir1(char* cmd, char* file, int i_o);
void ioRedir2(char* cmd, char* file_in, char* file_out);

typedef struct {
	int position;
	int pid;
	char* cmd;
} BGP;

int main(){
	//prompt user
	typedef BGP *bgps;
	char* userInput= malloc(sizeof(char)*50);
	int counter=0;
	int bgCount=-1;
	
	while(strcmp(userInput, "exit") != 0){
		prompt(userInput);
		if(bgCount>-1){
			checkBGP(bgCOunt);
		}
		if(strcmp(userInput,"") != 0){
			printf("Got the input: %s\n", userInput);
			//parse input
			//execute counter++ for every command executed
		}

	}
	free(userInput);
	free(bgps);
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

void bgProcess(char* data){
	pid_t pid = fork();
	if(pid == -1){
		//error
		exit(1);
	}
	else if(pid==0){
		
	} else {
		//parent
	}
}
void checkBGP(int counter){
	
}
void ioRedir1(char* cmd, char* file, int i_o){
	int fd, buff;
	char* fileData;
	if(i_o==0){ //output
		//redirect cmd result to file
		fd = open(file, O_CREAT | O_WRONLY);
		if(fd==-1){ //could not create
			printf("Error creating file: %s", file);
		}
		else {
			if(fork() == 0){
				close(STDOUT_FILENO);
				dup(fd);
				close(fd);
				printf("Output Child done!");
			}
			else {
				close(fd);
				printf("Output parent done!");
			}
		}
	}
	else if(i_o==1){ //input
		//check if file exists
		//error if it doesnt else
		//execute cmd with file as input
		fd = open(file, O_RDONLY);
		if(fd == -1){ //means error opening file
			printf("Error opening file: %s", file);
		}
		else{
			if(fork() == 0){
				//child
				close(STDIN_FILENO);
				dup(fd);
				close(fd);
				//execute process bscially it will read in from the file which is now "stdin"
				//call execution using fileData as input
				printf("Input Child done!");
			}
			else {
				//parent
				close(fd);
				printf("input parent done!");
			}
		}
	} else{
		//error
		printf("Error: bad syntax");
	}
	
}
void ioRedir2(char* cmd, char* file_in, char* file_out){
	//check if file_in exists error if it dont
	//cmd takes filein as input and saves result in fileout
}
