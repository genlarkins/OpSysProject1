#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
	char** tokens;
	int numTokens;
} instruction;

void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);

void prompt();

int main(){
	
	//prompt user
	int counter=0;
	
	char* token = NULL;
	char* temp = NULL;
	
	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;
	
	int loopcount = 0;
	int firstIns = 0;
	do {
		prompt();
		loopcount++;
		do {
			scanf("%ms",&token);
			
			temp = (char*) calloc(100, (strlen((token)+1) * sizeof(char)));
			if(loopcount == 1){
				if(token[0] == '\n'){
					continue;
				}
				else if(token[0] == '|' || token[0] == '<' || token[0] == '>') {
					printf("Error: Invalid command syntax\n");
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
		printTokens(&instr);
		if(strcmp(instr.tokens[0],"exit") != 0){ //if token not exit
			//execute command
			clearInstruction(&instr);
		}
		else {
		} //exit
	} while(loopcount == 1 || strcmp(instr.tokens[0],"exit") != 0);
	printf("Exiting now!\n");
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



void prompt(){
	char* user = getenv("USER");
	char* machine = getenv("MACHINE");
	char* pwd = getenv("PWD");
	printf("%s@%s: %s>", user, machine, pwd);
	//get the command a user inputs
}