#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

typedef struct {
	char** tokens;
	int numTokens;
} instruction;

void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void getTokens(instruction* instr_ptr, char** cmd);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);

void prompt();

//char* prompt();

//function to check existence of file
bool exists(const char *fname);

//function to check if file is regular
bool check_regular(const char *fname);

//function to execute commands
void my_execute(char **cmd);

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
		do {
			scanf("%ms",&token);
			temp = (char*) calloc(100, (strlen((token)+1) * sizeof(char)));
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
		printTokens(&instr);

		//EXECUTE COMMANDS
		//clearInstruction(&instr);
		//loop through to get all paths
		//if flag is still false, file is not executable
		bool file_flag = false;
		while(path != NULL){
			//printf( " %s\n", token );
			//path concatenated with provided pathname
			strcpy(concat, path);
			strcat(concat, "/");
			strcat(concat, instr.tokens[0]);
			//printf(" %s\n", concat);
			//printf(" %s\n", cmd[0]);
			if(exists(concat) && check_regular(concat)){
				//strcpy(concat, "");
				file_flag = true;
				break;
			}
			strcpy(concat, "");
			path = strtok(NULL, ":");
		}
		//clearInstruction(&instr);

    if(!file_flag)
      printf("%s\n", "File does not exist or is not regular");
    else {
      cmd[0] = concat;
      getTokens(&instr, cmd);
      my_execute(cmd);
    }
		//free(cmd);
    clearInstruction(&instr);
    //strcpy(userInput, "");
	}
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
    execv(cmd[0], cmd);
    printf("Problem executing %s\n", cmd[0]);
    exit(1);
  }
  else {
    //Parent
    waitpid(pid, &status, 0);
  }
}
