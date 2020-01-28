/*Madison Vandersluis
 *This will contain solutions for project 1
 *Specifically: prompt, I/O redirection, Background Processing
 *Due: Jan 28, 2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

void prompt(char* input);
void bgProcess(char* data, BGP **bgp);
void checkBGP(int counter, BGP **bgp);
void ioRedir1(char* cmd, char* file, int i_o);

typedef struct {
	int position;
	int pid;
	char** cmd;
} BGP;

int main(){
	//prompt user
	BGP **bgps;
	char* userInput= malloc(sizeof(char)*50);
	int counter=0;
	int bgCount=-1;
	
	while(strcmp(userInput, "exit") != 0){
		prompt(userInput);
		if(bgCount>-1){
			checkBGP(bgCount, &bgps);
		}
		if(strcmp(userInput,"") != 0){
			printf("Got the input: %s\n", userInput);
			//parse input
			//execute counter++ for every command executed
			//everytime bgprocess is run incr bgCount
			if(bgCount==-1){ //will only run once
				//run bgprocess
				char *cmd[3] = {"/bin/sleep","25", NULL};
				bgProcess(cmd, &bgps, &bgCount);
			}
		}
	}
	//Finish all bgprocesses
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

void bgProcess(char* data, BGP **bgp, int *counter){
	pid_t pid = fork();
	if(pid == -1){
		//error
		exit(1);
	}
	else if(pid==0){ 
		//make new process to add
		
		if (counter == -1){ //if no bgp yet
			bgp = (BGP**) calloc(100, sizeof(BGP*));
		}
		else { //otherwise add an additional one
			bgp = (BGP**) realloc(bgpP, (counter+2) * sizeof(BGP*)); //+2 bc counter starts at -1
		}
		//add new bgp
		bgp[counter+1] = (BGP *)calloc(100, (strlen(counter)+1) * sizeof(BGP));
		bgp[counter+1]->cmd = (char *)calloc(100, (strlen(data)+1) * sizeof(char));
		strcpy(bgpP[counter+1], data); 
		bgp[counter+1]->pid = pid;
		bgp[counter+1]->position = counter+1;
		*counter++;
		//add to bgProcesses
		//addProcess(bgp, newbgp, counter);
		printf("[%d]\t[%d]", newbgp->position, newbgp->pid);
		//execute
		execv(cmd[0],cmd); 
		fprint("Problem executing %s\n", data);
		exit(1);
	} else {
		//parent
		//dont wait
	}
}

void checkBGP(int counter, BGP **bgp){
	//if a process is finished then set its position to -1
	int i;
	for(i=0; i<counter+1; i++){
		if((bgp[i]) != NULL){
			//check if position is -1
			if(waitpid(bgp[i]->pid, &status, WNOHANG) == 0){ //not done
				//dont do anything
			}
			else if(waitpid(bgp[i]->pid, &status, WNOHANG) == bgp[i]->pid) {
				//is done
				printf("[%d]+\t", bgp[i]->position, bgp[i]->cmd);
				//change position to -1
			}
		}
	}
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
				wait(fd); //wait for child to finish
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
				wait(fd); //wait for child to finish
				close(fd);
				printf("input parent done!");
			}
		}
	} else{
		//error
		printf("Error: bad syntax");
	}	
}

