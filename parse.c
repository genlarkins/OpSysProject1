#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
	char** tokens;
	int numTokens;
} instruction;

void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);
char* shortcutResolution(char* path);
void error(instruction* instr_ptr);
bool checkEnvironmental(instruction* instr_ptr);
void expandEnvironmental(instruction* instr_ptr, char* expanded);
void pathParse(instruction* instr_ptr, char* fullpath);

int main() {
	char* token = NULL;
	char* temp = NULL;

	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;
	
	int loopcount = 0;
	int firstIns = 0;
	while (1) {
		printf("Please enter an instruction: ");
		loopcount++;
		// loop reads character sequences separated by whitespace
		do {
			//scans for next token and allocates token var to size of scanned token
			scanf("%ms", &token);
			temp = (char*)calloc(100,(strlen(token) + 1) * sizeof(char));

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
			for (i = 0; i < strlen(token); i++) {
				//pull out special characters and make them into a separate token in the instruction
				if (token[i] == '|' || token[i] == '>' || token[i] == '<' || token[i] == '&') {
					if (i-start > 0) {
						memcpy(temp, token + start, i - start);
						temp[i-start] = '\0';
						addToken(&instr, temp);
					}

					char specialChar[2];
					specialChar[0] = token[i];
					specialChar[1] = '\0';

					addToken(&instr,specialChar);

					start = i + 1;
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
		} while ('\n' != getchar());    //until end of line is reached


		addNull(&instr);
//		printTokens(&instr);

/* THIS IS HOW WE ACCESS TOKENS VVVVVVVV 
	    int i;
		  char * expanded = calloc(500, sizeof(char));
	    printf("Tokens:\n");
	    for (i = 0; i < instr.numTokens; i++)
		  {
        	    if ((instr.tokens)[i] != NULL)
				      {
					          printf("%s\n", (instr.tokens)[i]);
					          if(checkEnvironmental(&instr) == true)
					          {
						            expandEnvironmental(&instr, expanded);
						            printf("Expanded: %s\n", expanded);
					          }
	

				      }
		
       }
		free(expanded);
*/
		clearInstruction(&instr);

	}

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
		instr_ptr->tokens = (char**)malloc(sizeof(char*));
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
				expanded = getenv(environmental);
				free(environmental);
			}
		}
		
	}
}
