/*Madison Vandersluis
 *This will contain solutions for project 1
 *Specifically: prompt, I/O redirection, Background Processing
 *Due: Jan 28, 2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct {
	int position;
	pid_t pid;
	char* cmd;
} BGP;

typedef struct {
	BGP **bgpc;
} BGcontain;

void prompt(char* input);
void bgProcess(char** data, BGcontain *bgp, int* counter);
void checkBGP(int counter, BGcontain* bg);
void ioRedir(char** cmd, char* file, int i_o);
void ioRedir2(char** cmd, char* file1, char* file2);
void endBGP(int counter, BGcontain* bg);

int main(){
	//prompt user
	BGcontain bgps;
	char* userInput= malloc(sizeof(char)*50);
	int counter=0;
	int bgCount=-1;
	
	while(strcmp(userInput, "exit") != 0){
		prompt(userInput);
		if(bgCount>-1){
			checkBGP(bgCount, bgps);
		}
		if(strcmp(userInput,"") != 0){
			printf("Got the input: %s\n", userInput);
			//parse input
			//execute counter++ for every command executed
			//everytime bgprocess is run incr bgCount
			if(counter==0){ //will only run once
				//run bgprocess
				counter++;
				char* cmd[3] = {"/bin/sleep","20", NULL};
				bgProcess(cmd, &bgps, &bgCount);
			}
		}
	}
	//Finish all bgprocesses
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

void bgProcess(char** data, BGcontain *bgp, int *counter){
	pid_t pid = fork();
	int status;
	
	if (*counter == -1){ //if no bgp yet
		bgp->bgpc = (BGP**) calloc(100, sizeof(BGP*));
	}
	else { //otherwise add an additional one
		bgp->bgpc = (BGP**) realloc(bgp, (*counter+2) * sizeof(BGP*)); //+2 bc counter starts at -1
	}
	//add new process to bgp
	bgp->bgpc[*counter+1] = (BGP *)calloc(100, (sizeof(BGP)));
	bgp->bgpc[*counter+1]->cmd = (char *) calloc(100, (strlen(data[0])+1) * sizeof(char));
	strcpy(bgp->bgpc[*counter+1]->cmd, data[0]); 
	bgp->bgpc[*counter+1]->pid = pid;
	bgp->bgpc[*counter+1]->position = *counter+1;
	*counter = *counter + 1;
	printf("[%d]\t%d\n",*counter+1, pid);
	
	if(pid == -1){
		//error
		exit(1);
	}
	else if(pid==0){ 
		//make new process to add
		
		execv(data[0], data); 
		printf("Problem executing %s\n", data[0]);
		exit(1);
	} else {
		//parent
		//dont wait
		waitpid(-1, &status, WNOHANG);
	}
}

void checkBGP(int counter, BGcontain* bg){
	//if a process is finished then set its position to -1
	printf("Checking if bgp is done...\n");
	int status;
	int i;
	int g;
	//printf("b %d\n", bg.bgpc[0]->pid);
	for(i=0; i<counter+1; i++){
		g = waitpid(bg->bgpc[i]->pid, &status, WNOHANG);
		printf("a %d\n", g);
		//check if position is -1
		if(g == 0){ //not done
			//dont do anything
		}
		else if(g == bg->bgpc[i]->pid) {
			//is done
			printf("d\n");
			printf("[%d]+\t %s\n", bg->bgpc[i]->position, bg->bgpc[i]->cmd);
			bg->bgpc[i]->position = -1;
			//change position to -1
		}
	}
}


void ioRedir(char** cmd, char* file, int i_o){
	int fd, buff;
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
				execv(cmd[0],cmd); 
				printf("Problem executing %s\n", cmd[0]);
				exit(1);
			}
			else {
				close(fd);
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
				//execute process bascially it will read in from the file which is now "stdin"
				//call execution using fileData as input
				execv(cmd[0],cmd); 
				printf("Problem executing %s\n", cmd[0]);
				exit(1);
			}
			else {
				//parent
				close(fd);
			}
		}
	}
	else{
		//error
		printf("Error: bad syntax");
	}	
}

void ioRedir2(char** cmd, char* file1, char* file2){
	int fd1, fd2, buff;
	fd1 = open(file1, O_RDONLY);
	fd2 = open(file2, O_CREAT | O_WRONLY | O_TRUNC);
	if(fd1==-1 && fd2 ==-1){
		printf("Error opening one or more of the files.\n");
	} else {
		if(fork()==0){
			close(STDIN_FILENO);
			dup(fd1);
			close(fd1);
			close(STDOUT_FILENO);
			dup(fd2);
			close(fd2);
			execv(cmd[0],cmd); 
			printf("Problem executing %s\n", cmd[0]);
			exit(1);
		}
		else {
			close(fd1);
			close(fd2);
		}
	}
}

