#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>

typedef struct {
	char** tokens;
	int numTokens;
} instruction;

typedef struct {
	int position;
	pid_t pid;
	char* cmd;
} BGP;

typedef struct {
	BGP **bgpc;
} BGcontain;


bool exists(const char *fname); //function to check existence of file
bool check_regular(const char *fname); //function to check if file is regular


void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);
char * findPath(char * instr, char * path);

void prompt();
//will remove bgProcess later
void bgProcess(char** data, BGcontain *bgp, int* bgCounter, int* jobs);
void checkBGP(int bgCounter, BGcontain* bg, int* jobs, int* counter);
void clearBGcontain(BGcontain* bg);
void ioRedir(instruction instr, instruction cmd, char* file, int i_o, int trigger, BGcontain *bgp, int* bgCounter, int*counter, int* jobs);
void ioRedir2(instruction instr, char* file1, char* file2, int trigger, BGcontain *bgp, int* bgCounter, int*counter, int* jobs);
void printJobs(BGcontain *bgp, int jobs);

//every execution must have: BGcontain *bgp, int* bgCounter, int*counter, int* jobs, int bgTrigger

int main(){
	
	int counter=0;
	int jobs=0;
	int exeType;
	int bgTrigger;
	int ioCount=0;
	int error;
	int bgCount = -1;
	int exit=0;
	int loopcount = 0;
	int firstIns = 0;
	
	char* token = NULL;
	char* temp = NULL;
	
	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;
	
	BGcontain bgps;
	
	do {
		prompt(); //prmpts user to enter stuff
		loopcount++;
		exeType = 0;
		bgTrigger = 0;
		ioCount = 0;
		error = 0;
		do {
			//scans for next token and allocates token var to size of scanned token
			scanf("%ms",&token);
			if(loopcount == 1){
				if(token[0] == '\n'){
					continue;
				}
			}
			int i;
			int start = 0;
			
			if(token[0]=='&'){
				//shift everything to left so that first & is ignored
				temp = (char*) calloc(100, (strlen((token)) * sizeof(char)));
			}
			else{
				temp = (char*) calloc(100, (strlen((token)+1) * sizeof(char)));
			}
			
			for (i=0; i<strlen(token); i++) {
				if(token[i] == '|' || token[i] == '>' || token[i] == '<'
				   || token[i] == '&') {
					if(token[i] == '|'){
						exeType = 2;
					}
					else if((token[i] == '<' || token[i] == '>')) {
						exeType = 1;
						ioCount = ioCount + 1;
					}
					else if(token[i]=='&'){
						bgTrigger = bgTrigger + 1;
					}
					
					if(i-start > 0){
						memcpy(temp, token + start, i-start);
						temp[i-start] = '\0';
						addToken(&instr, temp);
					}
					char specialChar[2];
					specialChar[0] = token[i];
					specialChar[1] = '\0';
					addToken(&instr,specialChar);
					start = i+1;
				}
			}
			if (start < strlen(token)) {
				memcpy(temp, token + start, strlen(token) - start);
				temp[i-start] = '\0';
				addToken(&instr, temp);
			}
			//free and reset variables
			free(token);
			free(temp);
			token = NULL;
			temp = NULL;
		} while ('\n' != getchar());
		
		addNull(&instr);
		printTokens(&instr);
		
		//bgcheck
		if(jobs>0){
			checkBGP(bgCount, &bgps, &jobs, &counter);
		}
		
		if(instr.tokens[0] == NULL || strcmp(instr.tokens[0],"exit") != 0){ //if token not exit
			//execute command
			int j;
			if(instr.tokens[0] != NULL && (strcmp(instr.tokens[0],"|")==0 || strcmp(instr.tokens[0],"<")==0
													|| strcmp(instr.tokens[0],">")==0)) {
					printf("Error: Invalid command syntax\n");
					error++;
			}
			for(j=0; j<instr.numTokens; j++){
				if(instr.tokens[j] != NULL && instr.tokens[j+1] != NULL && strcmp(instr.tokens[j],"&") == 0
				   && j!=0){
					printf("Error: Invalid command syntax\n");
					error++;
				}
				else if(instr.tokens[j] != NULL && (strcmp(instr.tokens[j],"<")==0
											   || strcmp(instr.tokens[j],">")==0)){
					if(instr.tokens[j+1] == NULL || strcmp(instr.tokens[j+1],"<")==0
					   || strcmp(instr.tokens[j+1],">")==0){
						printf("Error: Invalid command syntax\n");
						error++;
					}
				} 
			}
			if(error == 0 || instr.tokens[0] == NULL){
				if(strcmp(instr.tokens[0], "jobs") == 0){
					if(jobs==0){
						printf("No active background processes running.\n");
					}
					else{
						int i;
						for(i=0; i<bgCount+1;i++){
							if(bgps.bgpc[i]->position != -1){
							printf("[%d]+ [%d] [%s]\n", bgps.bgpc[i]->position, bgps.bgpc[i]->pid, bgps.bgpc[i]->cmd);
							}
						}
					}
				}
				else if(exeType == 0){
					//normal execution
                    char ** cmd = calloc(1000, sizeof(char));
                    char buf[PATH_MAX];                                                                                                    
                    char* fullPath = calloc(512, sizeof(char *));
                    if(check_built_in(&instr, cmd)){
                        getTokens(&instr, cmd);
                        my_execute(cmd);
                        /*file_flag = true;
                        built_in = true;*/
                        //break;
                    }
                    else if(fullPath = realpath(instr.tokens[0], buf)){
                        printf("%s\n", fullPath);
                        cmd[0] = fullPath;
                        getTokens(&instr, cmd);
                        my_execute(cmd);
                    }
                    else{
                        printf("%s\n", "Error: Invalid command");
                    }
				}
				else if(exeType == 1){
					// io redirection
					//cmd <|> file <|> file
					int z;
					int pos=0;
					instruction ioR;
					for(z=0; z<instr.numTokens; z++){
						if(instr.tokens[z] != NULL && pos==0 && (strcmp(instr.tokens[z],"<") == 0 || strcmp(instr.tokens[z],">")==0)){
							pos = z;
						} else {
							if(z!=0){
								if(instr.tokens[z] != NULL && (strcmp(instr.tokens[z],"<") !=0 || strcmp(instr.tokens[z],">")!=0) &&
									(strcmp(instr.tokens[z-1],"<")==0 || strcmp(instr.tokens[z-1],">")==0)){
									printf("%s\n", instr.tokens[z]);
									addToken(&ioR, instr.tokens[z]);
								}
							} else { //if first always add
								addToken(&ioR, instr.tokens[z]);
							}
							
						}
					}
					addNull(&ioR);
					if(ioCount == 1){
						if(strcmp(instr.tokens[pos],">")==0){ //output
							printf("single output\n");
							ioRedir(ioR, instr.tokens[pos+1], 0, bgTrigger, &bgps, &bgCount, &counter, &jobs);
						}
						else if(strcmp(instr.tokens[pos],"<")==0) {
							printf("single input: %s\n", instr.tokens[pos]);
							ioRedir(ioR, instr.tokens[pos+1], 1, bgTrigger, &bgps, &bgCount, &counter, &jobs);
						}
					} else {
						if(strcmp(instr.tokens[pos],"<")==0){ //input
							printf("put first\n");
							ioRedir2(ioR, instr.tokens[pos+1], instr.tokens[pos+3], bgTrigger, &bgps, &bgCount, &counter, &jobs);
						}
						else if(strcmp(instr.tokens[pos],">")==0){
							printf("output first\n");
							ioRedir2(ioR, instr.tokens[pos+3], instr.tokens[pos+1], bgTrigger, &bgps, &bgCount, &counter, &jobs);
						}
					}
					
				}
				else if(exeType == 2){
					// pipes
				}
			}
		}
		
		if(strcmp(instr.tokens[0],"exit") == 0){
			exit = 1;
		}
		
		clearInstruction(&instr);
	} while(exit == 0);
	//empty bgprocesses
	
	printf("Exiting now!\n\tCommands executed: %d\n",counter);
	//clear bgp
	return 0;
}

//reallocates instruction array to hold another token
//allocates for new token within instruction array
void addToken(instruction* instr_ptr, char* tok)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**) calloc(100,sizeof(char*));
	else
		instr_ptr->tokens = (char**) realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

	//allocate char array for new token in new slot
	instr_ptr->tokens[instr_ptr->numTokens] = (char *)malloc((strlen(tok)+1) * sizeof(char));
	strcpy(instr_ptr->tokens[instr_ptr->numTokens], tok);

	instr_ptr->numTokens++;
}

void addNull(instruction* instr_ptr)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**)calloc(100, sizeof(char*));
	else
		instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

	instr_ptr->tokens[instr_ptr->numTokens] = (char*) NULL;
	instr_ptr->numTokens++;
}

void printTokens(instruction* instr_ptr)
{
	int i;
	printf("Tokens:\n");
	for (i = 0; i < instr_ptr->numTokens; i++) {
		if ((instr_ptr->tokens)[i] != NULL)
			printf("%s\n", (instr_ptr->tokens)[i]);
	}
}

void clearInstruction(instruction* instr_ptr)
{
	int i;
	for (i = 0; i < instr_ptr->numTokens; i++)
		free(instr_ptr->tokens[i]);

	free(instr_ptr->tokens);

	instr_ptr->tokens = NULL;
	instr_ptr->numTokens = 0;
}

void prompt(){ //prompts the user
	char* user = getenv("USER");
	char* machine = getenv("MACHINE");
	char* pwd = getenv("PWD");
	printf("%s@%s: %s>", user, machine, pwd);
}

void checkBGP(int bgCounter, BGcontain* bg, int* jobs, int* counter){
	//if a process is finished then set its position to -1
	printf("Checking if bgp is done...\n");
	int status;
	int i;
	int g;
	//printf("b %d\n", bg.bgpc[0]->pid);
	for(i=0; i<*counter+1; i++){
		g = waitpid(bg->bgpc[i]->pid, &status, WNOHANG);
		//check if position is -1
		if(g == 0){ //not done
			//dont do anything
		}
		else if(g == bg->bgpc[i]->pid) {
			//is done
			printf("[%d]+\t %s\n", bg->bgpc[i]->position, bg->bgpc[i]->cmd);
			bg->bgpc[i]->position = -1;
			*counter = *counter + 1;
			*jobs = *jobs - 1;
			//change position to -1
		}
	}
}


void ioRedir(instruction instr, instruction cmd, char* file, int i_o, int trigger, BGcontain* bgp, int* bgCounter, int* counter, int* jobs){ //when there's just input or output
	int fd, status;
	if(i_o==0){ //output
		//redirect cmd result to file
		fd = open(file, O_CREAT | O_WRONLY | O_TRUNC);
		if(fd==-1){ //could not create
			printf("Error creating file: %s\n", file);
		}
		else {
			pid_t pid = fork();
			if(pid == 0){
				close(STDOUT_FILENO);
				dup(fd);
				close(fd);
				instr.tokens[0] = (char*) realloc(instr.tokens[0], strlen(findPath(instr.tokens[0], getenv("PATH"))) * sizeof(char));
				strcpy(instr.tokens[0], findPath(instr.tokens[0], getenv("PATH")));
				execv(instr.tokens[0],instr.tokens);
				printf("Problem executing %s\n", instr.tokens[0]);
				exit(1);
			}
			else {
				waitpid(pid, &status, 0);
				close(fd);
				*counter = *counter+1;
			}
		}
	}
	else if(i_o==1){ //input
		//check if file exists
		//error if it doesnt else
		//execute cmd with file as input
		fd = open(file, O_RDONLY);
		if(fd == -1){ //means error opening file
			printf("Error opening file: %s\n", file);
		}
		else{
			pid_t pid = fork();
			if(pid == 0){
				//child
				close(STDIN_FILENO);
				dup(fd);
				close(fd);
				//execute process bascially it will read in from the file which is now "stdin"
				//call execution using fileData as input
				instr.tokens[0] = (char*) realloc(instr.tokens[0], (strlen(findPath(instr.tokens[0], getenv("PATH")))+1) * sizeof(char));
				strcpy(instr.tokens[0], findPath(instr.tokens[0], getenv("PATH")));
				printf("Path is Now: %s\n", instr.tokens[0]);
				execv(instr.tokens[0],instr.tokens); 
				printf("Problem executing %s\n", instr.tokens[0]);
				exit(1);
			}
			else {
				//parent
				waitpid(pid, &status, 0);
				close(fd);
				*counter = *counter+1;
				if(trigger!=0){
				//bg stuff
				if (*bgCounter == -1){ //if no bgp yet
					bgp->bgpc = (BGP**) calloc(100, sizeof(BGP*));
				}
				else { //otherwise add an additional one
					bgp->bgpc = (BGP**) realloc(bgp, (*bgCounter+2) * sizeof(BGP*)); //+2 bc counter starts at -1
				}
				//add new process to bgp
				bgp->bgpc[*bgCounter+1] = (BGP *)calloc(100, (sizeof(BGP)));
				bgp->bgpc[*bgCounter+1]->cmd = (char *) calloc(100, (strlen(cmd)+1) * sizeof(char));
				strcpy(bgp->bgpc[*bgCounter+1]->cmd, cmd); 
				bgp->bgpc[*bgCounter+1]->pid = pid;
				bgp->bgpc[*bgCounter+1]->position = *bgCounter+1;
				*counter = *bgCounter + 1;
				printf("[%d] [%d]\n",*bgCounter+1, pid);
			} else{
				*counter = *counter + 1;
				waitpid(pid, &status, 0);
			}
			}
		}
	}
	else{
		//error
		printf("Error: bad syntax");
	}	
}
void ioRedir2(instruction instr, char* file1, char* file2, int trigger, BGcontain *bgp, int* bgCounter, int* counter, int* jobs){ //for when theres an input and outputint fd1, fd2, buff;
	int fd1, fd2;
	fd1 = open(file1, O_RDONLY);
	fd2 = open(file2, O_CREAT | O_WRONLY | O_TRUNC);
	int status;
	if(fd1==-1 && fd2 ==-1){
		printf("Error opening one or more of the files.\n");
	} else {
		pid_t pid = fork();
		//make cmmd
		int lengthCMD=0;
		char* cmd;
		int x;
		for(x=0; x<instr.numTokens;x++){
			if(instr.tokens[x] != NULL){
				lengthCMD = lengthCMD + strlen(instr.tokens[x]) + 1;
			}
		}
		cmd = (char*) calloc(100, (lengthCMD + 1)*sizeof(char));
		for(x=0; x<instr.numTokens;x++){
			if(instr.tokens[x] != NULL){
				strcat(cmd, instr.tokens[x]);
			}
		}
		
		if(pid==0){
			close(STDIN_FILENO);
			dup(fd1);
			close(fd1);
			close(STDOUT_FILENO);
			dup(fd2);
			close(fd2);
			instr.tokens[0] = (char*) realloc(instr.tokens[0], strlen(findPath(instr.tokens[0], getenv("PATH"))) * sizeof(char));
			strcpy(instr.tokens[0], findPath(instr.tokens[0], getenv("PATH")));
			execv(instr.tokens[0],instr.tokens); 
			printf("Problem executing %s\n", cmd);
			exit(1);
		}
		else {
			close(fd1);
			close(fd2);
			if(trigger!=0){
				//bg stuff
				if (*bgCounter == -1){ //if no bgp yet
					bgp->bgpc = (BGP**) calloc(100, sizeof(BGP*));
				}
				else { //otherwise add an additional one
					bgp->bgpc = (BGP**) realloc(bgp, (*bgCounter+2) * sizeof(BGP*)); //+2 bc counter starts at -1
				}
				//add new process to bgp
				bgp->bgpc[*bgCounter+1] = (BGP *)calloc(100, (sizeof(BGP)));
				bgp->bgpc[*bgCounter+1]->cmd = (char *) calloc(100, (strlen(cmd)+1) * sizeof(char));
				strcpy(bgp->bgpc[*bgCounter+1]->cmd, cmd); 
				bgp->bgpc[*bgCounter+1]->pid = pid;
				bgp->bgpc[*bgCounter+1]->position = *bgCounter+1;
				*counter = *bgCounter + 1;
				printf("[%d] [%d]\n",*bgCounter+1, pid);
			} else{
				*counter = *counter + 1;
				waitpid(pid, &status, 0);
			}
		}
	}
}

char * findPath(char * instr, char * path)
{
	int size = 5;
	char * newpath;
	char *pathcopy = (char*) calloc(100, (strlen(path)+1)*sizeof(char));
	strcpy(pathcopy, path);
	
	newpath = strtok(pathcopy, ":");
	char * concat2 = (char *)calloc(100, sizeof(char)*size);
	bool file_flag1 = false;
	while(newpath != NULL)
	{
		strcpy(concat2, newpath);
		strcat(concat2, "/");
		strcat(concat2, instr);
		if(exists(concat2) && check_regular(concat2))
		{
			file_flag1 = true;
			break;
		}
		strcpy(concat2,"");
		newpath = strtok(NULL, ":");
	
	}
	if(file_flag1 == true)
		return(concat2);
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

void getTokens(instruction* instr_ptr, char** cmd)
{
        int i;
        for(i =1; i < instr_ptr->numTokens; i++){
                if ((instr_ptr->tokens)[i-1] != NULL)
                        cmd[i] = "";
        }
        for(i =1; i < instr_ptr->numTokens; i++){
                if ((instr_ptr->tokens)[i-1] != NULL)
                        cmd[i] = instr_ptr->tokens[i];
        }
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
                if(cmd[0] == "echo"){
                        if(strncmp(cmd[1], "$", 1) == 0){
                                cmd[1]++;
                                if(getenv(cmd[1]) != NULL){
                                        cmd[1] = getenv(cmd[1]);
                                }
                                else {
                                        printf("%s\n", "Error: Environmental variable does not exist!");
                                        exit(1);
                                }
                        }
                        printf("%s\n", cmd[1]);
                        exit(0);
                }
                else if(cmd[0] == "cd"){
                        //printf("%s\n", cmd[1]);
                        if((strcmp(cmd[1], "$HOME") == 0) || cmd[1] == ""){
                                printf("%s\n", cmd[1]);
                                if(chdir(getenv("HOME")) == 0){
                                        setenv("PWD", getenv("HOME"), 1);
                                }
                        }
                        else{
                                char* fullpath = calloc(512, sizeof(char *));
                                char* ptr = calloc(512, sizeof(char *));
                                //ptr = realpath(cmd[1], fullpath);
                                //printf("%s\n", ptr);
                                //free(fullpath);
                                //free(ptr);
                                if((ptr = realpath(cmd[1], fullpath)) != NULL){
                                        if(chdir(ptr)== 0){
                                                setenv("PWD", ptr, 1);
                                            }
                                }
                                else {
                                        printf("%s\n", "Error: Directory does not exist!");
                                }
                        }
                }
                else {
        execv(cmd[0], cmd);
        printf("Problem executing %s\n", cmd[0]);
        exit(1);
                }
  }
  else {
    //Parent
    waitpid(pid, &status, 0);
  }
}