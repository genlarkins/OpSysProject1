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

void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void getTokens(instruction* instr_ptr, char** cmd);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);
char* shortcutResolution(char* path);
void error(instruction* instr_ptr);
bool checkEnvironmental(instruction* instr_ptr);
void expandEnvironmental(instruction* instr_ptr, char* expanded);
void pathParse(char* path, char* fullpath);
char * findPath(char * instr, char * path);

void prompt();

//char* prompt();

//function to check existence of file
bool exists(const char *fname);

//function to check if file is regular
bool check_regular(const char *fname);

//function to execute commands
void my_execute(char **cmd);

//function to check if command is built in
bool check_built_in(instruction* instr_ptr, char** cmd);

int main(){

	int size = 4;
	char *userInput = (char *)malloc(sizeof(char)*size);
	int counter=0;

	char* token = NULL;
	char* temp = NULL;

	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;

	//$PATH
	char * path = strtok(getenv("PATH"), ":");
	char * path2 = calloc(1000, sizeof(char)); 

	strcpy(path2, path);
	//string for concatonated path names
	char * concat = (char *)malloc(sizeof(char)*size);
	//string containing command + args
	char ** cmd = (char **)malloc(sizeof(char)*size);

	int loopcount = 0;
	int firstIns = 0;

	while(strcmp(userInput, "exit") != 0){
		//PROMPT USER
		strcpy(userInput, "");
		//userInput = prompt();
		prompt();
		//PARSE INPUT
		loopcount++;
		do {
			scanf("%ms",&token);
			temp = (char*) calloc(100, (strlen((token)+1) * sizeof(char)));

			if(loopcount == 1)
			{
							//makes sure first thing entered is not a pipe
							if(token[0] == '|')
							{
								printf("Invalid command!\n");
								continue;
							}
			}

			int i;
			int start = 0;
			for (i=0; i<strlen(token); i++) {
				if(token[i] == '|' || token[i] == '>' || token[i] == '<'
				   || token[i] == '&') {
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
		//getTokens(&instr, cmd);
		//printTokens(&instr);

		char * enteredpath = calloc(1000, sizeof(char));
				char * input = (instr.tokens)[1];
		    sprintf(enteredpath, input);

				char * fullpath = calloc(512, sizeof(char *));

				//THE INPUT FOR THIS FUNCTION HAS TO BE DYNAMICALLY ALLOCATED SO YOU NEED TO MAKE A DYNAMIC VARIABLE THEN SET IT
				//EQUAL TO THE TOKEN(AKA PATH) YOU WANT TO SEND IN
				pathParse(enteredpath, fullpath);
				printf("Full path: %s\n",fullpath);


				free(fullpath);
		    free(enteredpath);


		//EXECUTE COMMANDS
		//loop through to get all paths
		//if flag is still false, file is not executable
		//don't repeat execution for built ins
		bool file_flag = false;
		bool built_in = false;
		//strcpy(path2, path);
		/*while(path2 != NULL){
			//path concatenated with provided pathname unless it is a built-in
			if(check_built_in(&instr, cmd)){
				getTokens(&instr, cmd);
				my_execute(cmd);
				file_flag = true;
				built_in = true;
				break;
			}
			else {
				printf("%s\n", getenv("PATH"));
				//printf("%s\n", path);
				strcpy(concat, path2);
				strcat(concat, "/");
				strcat(concat, instr.tokens[0]);
				printf("%s\n", concat);
			}
			if(exists(concat) && check_regular(concat)){
				file_flag = true;
				break;
			}
			strcpy(concat, "");
			//strcpy(path2, path);
			path2 = strtok(NULL, ":");
		}*/
    
    char buf[PATH_MAX];
    char* fullPath = calloc(512, sizeof(char *));
    //char* thePath = calloc(512, sizeof(char *));
    if(check_built_in(&instr, cmd)){
	getTokens(&instr, cmd);
	my_execute(cmd);
	file_flag = true;
	built_in = true;
	//break;
    }
    else {
      	//ptr = realpath(instr.tokens[0], thePath); 
	//fullPath = findPath(instr.tokens[0], getenv("PATH"));
	fullPath = findPath(instr.tokens[0], getenv("PATH"));
	printf("%s\n", fullPath);
        cmd[0] = fullPath;
	getTokens(&instr, cmd);
      	my_execute(cmd);
    }
    /*if(!file_flag){
      printf("%s\n", "File does not exist or is not regular");
      //printf("%s\n", concat);
    }*/
    /*else if (!built_in){
      cmd[0] = concat;
      getTokens(&instr, cmd);
      my_execute(cmd);
    }*/
    clearInstruction(&instr);
	}
	//clearInstruction(&instr);
	free(userInput);
	printf("Exiting now!\n");
	return 0;
}

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

void clearInstruction(instruction* instr_ptr)
{
	int i;
	for (i = 0; i < instr_ptr->numTokens; i++)
		free(instr_ptr->tokens[i]);

	free(instr_ptr->tokens);

	instr_ptr->tokens = NULL;
	instr_ptr->numTokens = 0;
}

void prompt(){
	char* user = getenv("USER");
	char* machine = getenv("MACHINE");
	char* pwd = getenv("PWD");
	printf("%s@%s: %s>", user, machine, pwd);
	//get the command a user inputs
}

/*char* prompt(){
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
}*/

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

bool check_built_in(instruction* instr_ptr, char** cmd){
	if((strcmp(instr_ptr->tokens[0], "echo") == 0)){
		cmd[0] = "echo";
		return true;
	}
	if((strcmp(instr_ptr->tokens[0], "exit") == 0)){
		cmd[0] = "exit";
		return true;
	}
	if((strcmp(instr_ptr->tokens[0], "cd") == 0)){
		cmd[0] = "cd";
		return true;
	}
	if((strcmp(instr_ptr->tokens[0], "jobs") == 0)){
		cmd[0] = "jobs";
		return true;
	}
	return false;
}

bool checkEnvironmental(instruction* instr_ptr)
{
	int count = 0;
	int i;
	for (i = 0; i < instr_ptr->numTokens; i++)
	{
		if((instr_ptr->tokens)[i] != NULL)
			if((instr_ptr->tokens)[i][0] == '$')
			{
				count++;
				return true;
			}

	}
	if (count == 0)
		return false;
}
void expandEnvironmental(instruction* instr_ptr, char* expanded)
{
	char * environmental = calloc(1000, sizeof(char));

	int i;
	for (i = 0; i < instr_ptr->numTokens; i++)
	{
		if ((instr_ptr->tokens)[i] != NULL)
		{
			if(sscanf((instr_ptr->tokens)[i], "$%s", environmental) == 1)
			{
				sprintf(expanded, "%s", getenv(environmental));
	//			expanded = getenv(environmental);
				free(environmental);
			}
		}

	}
}

void pathParse(char* path, char* fullpath)
{
	    char currdir[]="/./";
	char currdiralone[]=".";
	char currdirstart[]="./";
	char currdirend[]="/.";
	char prevdir[]="/../";
	char prevdiralone[]="..";
	char prevdirstart[]="../";
	char prevdirend[]= "/..";
	char home[]="~/";
	char homealone[] = "~";
	char homewrong1[]="/~/";
	char homewrong2[]="/~";
    const char s[2] = "/";
	char periods[3] = "..";

	int wronghome = 0; //flag that indicates an error in the path was found

    char * tokcopy = calloc(1000, sizeof(char));
    char * strcp = calloc(1000, sizeof(char));
	char * currentdirectory = calloc(1000, sizeof(char));

    strcpy(strcp, path);
	int startflag = 0; // flag that if set to one says that the path starts with a /
	if(strcp[0] == '/')
		startflag = 1;

    char * curr;
	char * curralone;
	char * currstart;
	char * currend;
    curr = strstr(path, currdir);
	curralone = strstr(path, currdiralone);
	currstart = strstr(path, currdirstart);
	currend = strstr(path, currdirend);

	char * prev;
	char * prevalone;
	char * prevstart;
	char * prevend;
	prev = strstr(path, prevdir);
	prevalone = strstr(path, prevdiralone);
	prevstart = strstr(path, prevdirstart);
	prevend = strstr(path, prevdirend);

	char * ho;
	char * hoalone;
	char * howrong1;
	char * howrong2;
	ho = strstr(path, home);
	hoalone = strstr(path, homealone);
	howrong1 = strstr(path, homewrong1);
	howrong2 = strstr(path, homewrong2);

	int currflag = 0; //set to one if . appears in path
	int prevflag = 0; //set to one if .. appears in path


	sprintf(currentdirectory, "%s", getenv("PWD"));
	int x;
	int slashcount = 0;
	int checkslash = 0;
	int newlength = 0;
	for(x=0; x <= strlen(currentdirectory); x++)
		if(currentdirectory[x] == '/')
			slashcount++;
	for(x=0; x < strlen(currentdirectory); x++)
	{
		if(currentdirectory[x] == '/')
			checkslash++;
		if(checkslash == slashcount)
		{
			newlength = x;
			break;
		}

	}
	char previousdirectory[newlength+1];
	for(x=0; x<newlength; x++)
	{
		previousdirectory[x] = currentdirectory[x];
	}
//	printf("Current directory: %s\n", getenv("PWD"));
//	printf("Previous directory: %s\n", previousdirectory);


	if(startflag == 0 && strcp[0] != '.' && strcp[0] != '~')
	{
		sprintf(fullpath,"%s", getenv("PWD"));
		strcat(fullpath, s);
	//	strcat(fullpath, path);
	}


	if(prev)
	{
		if(strcmp(currentdirectory,s)==0)
			sprintf(fullpath, "%s", "Error! In root directory!");

		else
		{
			prevflag = 1;
			strcpy(prev, previousdirectory);
			sprintf(fullpath, "%s", path);
			char * newtok;
			newtok = strtok(strcp,s);

			int trigflad = 0;
			while(newtok != NULL)
			{
				strcpy(tokcopy, newtok);
				if(strcmp(tokcopy,periods)==0)
				{
					trigflad = 1;

				}


				if(trigflad == 1)
				{
					if(strcmp(tokcopy,periods)!=0)
					{
						strcat(fullpath,s);
						strcat(fullpath, tokcopy);
					}
				}

				newtok = strtok(NULL,s);
			}
		}

	}
	else if(prevstart && prevflag == 0)
	{
		if(strcmp(currentdirectory,s)==0)
			sprintf(fullpath, "%s", "Error! In root directory!");
		else
		{
			prevflag = 1;
			strcpy(prevstart, previousdirectory);
			sprintf(fullpath, "%s", path);
			char * newtok;
			newtok = strtok(strcp,s);

			int trigflad = 0;
			while(newtok != NULL)
			{
				strcpy(tokcopy, newtok);
				if(strcmp(tokcopy,periods)==0)
				{
					trigflad = 1;

				}


				if(trigflad == 1)
				{
					if(strcmp(tokcopy,periods)!=0)
					{
						strcat(fullpath,s);
						strcat(fullpath, tokcopy);
					}
				}

				newtok = strtok(NULL,s);
			}

		}
	}
	else if(prevend && prevflag == 0)
	{
		if(strcmp(currentdirectory,s)==0)
			sprintf(fullpath, "%s", "Error! In root directory!");
		else
		{
			prevflag = 1;
			strcpy(prevend, previousdirectory);
			sprintf(fullpath, "%s", path);
			char * newtok;
			newtok = strtok(strcp,s);

			int trigflad = 0;
			while(newtok != NULL)
			{
				strcpy(tokcopy, newtok);
				if(strcmp(tokcopy,periods)==0)
				{
					trigflad = 1;

				}


				if(trigflad == 1)
				{
					if(strcmp(tokcopy,periods)!=0)
					{
						strcat(fullpath,s);
						strcat(fullpath, tokcopy);
					}
				}

				newtok = strtok(NULL,s);
			}
		}

	}
	else if(prevalone && prevflag == 0)
	{
		if(strcmp(currentdirectory,s)==0)
			sprintf(fullpath, "%s", "Error! In root directory!");
		else
		{
			prevflag = 1;
			strcpy(prevalone, previousdirectory);
			sprintf(fullpath, "%s", path);
			char * newtok;
			newtok = strtok(strcp,s);

			int trigflad = 0;
			while(newtok != NULL)
			{
				strcpy(tokcopy, newtok);
				if(strcmp(tokcopy,periods)==0)
				{
					trigflad = 1;

				}


				if(trigflad == 1)
				{
					if(strcmp(tokcopy,periods)!=0)
					{
						strcat(fullpath,s);
						strcat(fullpath, tokcopy);
					}
				}

				newtok = strtok(NULL,s);
			}
		}
	}

	if(curr)
    {
		currflag = 1;
		strcpy(curr,getenv("PWD"));
		sprintf(fullpath, "%s", path);
		char * newtok;
		newtok = strtok(strcp,s);

		int trigflad = 0;
		while(newtok != NULL)
		{
			strcpy(tokcopy, newtok);
			if(*newtok =='.')
				trigflad = 1;


			if(trigflad == 1)
			{
				if(*newtok != '.')
				{
					strcat(fullpath,s);
					strcat(fullpath, tokcopy);
				}
			}

			newtok = strtok(NULL,s);
		}


	}
	else if(currstart && currflag == 0 && prevflag == 0)
	{
		currflag = 1;
		strcpy(currstart,getenv("PWD"));
		sprintf(fullpath, "%s", path);
		char * newtok;
		newtok = strtok(strcp,s);

		int trigflad = 0;
		while(newtok != NULL)
		{
			strcpy(tokcopy, newtok);
			if(*newtok =='.')
				trigflad = 1;


			if(trigflad == 1)
			{
				if(*newtok != '.')
				{
					strcat(fullpath,s);
					strcat(fullpath, tokcopy);
				}
			}

			newtok = strtok(NULL,s);
		}
	}
	else if(currend && currflag == 0 && prevflag == 0)
	{
		currflag = 1;
		strcpy(currend,getenv("PWD"));
		sprintf(fullpath, "%s", path);
		char * newtok;
		newtok = strtok(strcp,s);

		int trigflad = 0;
		while(newtok != NULL)
		{
			strcpy(tokcopy, newtok);
			if(*newtok =='.')
				trigflad = 1;


			if(trigflad == 1)
			{
				if(*newtok != '.')
				{
					strcat(fullpath,s);
					strcat(fullpath, tokcopy);
				}
			}

			newtok = strtok(NULL,s);
		}
	}
	else if(curralone && currflag == 0 && prevflag == 0)
	{
		currflag = 1;
		strcpy(curralone,getenv("PWD"));
		sprintf(fullpath, "%s", path);
		char * newtok;
		newtok = strtok(strcp,s);

		int trigflad = 0;
		while(newtok != NULL)
		{
			strcpy(tokcopy, newtok);
			if(*newtok =='.')
				trigflad = 1;


			if(trigflad == 1)
			{
				if(*newtok != '.')
				{
					strcat(fullpath,s);
					strcat(fullpath, tokcopy);
				}
			}

			newtok = strtok(NULL,s);
		}
	}

	if(ho)
	{

		strcpy(ho,getenv("HOME"));
		sprintf(fullpath, "%s", path);
		char * newtok;
		newtok = strtok(strcp,s);

		int trigflad = 0;
		while(newtok != NULL)
		{
			strcpy(tokcopy, newtok);
			if(*newtok =='~')
				trigflad = 1;


			if(trigflad == 1)
			{
				if(*newtok != '~')
				{
					strcat(fullpath,s);
					strcat(fullpath, tokcopy);
				}
			}

			newtok = strtok(NULL,s);
		}
	}


	if(howrong1 || howrong2)
	{
		wronghome = 1;
		char error[] = "Error! '~' has to come at beginning of path!";
		sprintf(fullpath,"%s\n", error);
	}
	else if(hoalone && strlen(strcp) == 1 && wronghome == 0)
	{
		strcpy(hoalone,getenv("HOME"));
		sprintf(fullpath, "%s", path);
		char * newtok;
		newtok = strtok(strcp,s);

		int trigflad = 0;
		while(newtok != NULL)
		{
			strcpy(tokcopy, newtok);
			if(*newtok =='~')
				trigflad = 1;


			if(trigflad == 1)
			{
				if(*newtok != '~')
				{
					strcat(fullpath,s);
					strcat(fullpath, tokcopy);
				}
			}

			newtok = strtok(NULL,s);
		}
	}







    free(strcp);
    free(tokcopy);
}

char * findPath(char * instr, char * path)
{
	int size = 5;
	char * newpath;
	char *pathcopy = (char*) calloc(1000, sizeof(char));
	strcpy(pathcopy, path);


	newpath = strtok(pathcopy, ":");
	char * concat2 = (char *)malloc(sizeof(char)*size);
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
