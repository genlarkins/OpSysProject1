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
void pathParse(char* path, char* fullpath);

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
