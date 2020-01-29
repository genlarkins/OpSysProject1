// needs addToken, addNull, clearInstruction, exists, and check_regular functions

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>

void singlePipe(instruction* instr2_ptr, instruction* instr3_ptr);
void doublePipe(instruction* instr4_ptr, instruction* instr5_ptr, instruction* instr6_ptr);
char * findPath(char * instr, char * path);


//in main need to go to if (token[i] == '|' || token[i] == '>' || token[i] == '<' || token[i] == '&') and add
// if(token[i] == '|')
//						pipecount++;

//in main after reading in the command

		instruction instr1;
        instr1.tokens = NULL;
        instr1.numTokens = 0;
        instruction instr2;
        instr2.tokens = NULL;
        instr2.numTokens = 0;
        instruction instr3;
        instr3.tokens = NULL;
        instr3.numTokens = 0;
		instruction instr4;
        instr4.tokens = NULL;
        instr4.numTokens = 0;
        instruction instr5;
        instr5.tokens = NULL;
        instr5.numTokens = 0;
		instruction instr6;
        instr6.tokens = NULL;
        instr6.numTokens = 0;

	int i;
	for(i = 0; i < instr.numTokens; i++)
	{
		if(instr.tokens[i] != NULL)
			addToken(&instr1, instr.tokens[i]);
	}
	addNull(&instr1);

	char * path1 = getenv("PATH");


	
    if(pipecount == 1 && (*instr.tokens[0] == '|' || *instr.tokens[instr.numTokens-2] == '|'))
		printf("Error! Invalid pipe command!\n");
	else if(pipecount == 1)
	{

		char * final1 = (char*) malloc(size*sizeof(char));
		char * final2 = (char*) malloc(size*sizeof(char));

		final1 = findPath(instr1.tokens[0], path1);

		addToken(&instr2,final1);

		int foundPipe = 0;
		char * pi = "|";
		int afterpipespot;
		int x;

		for(x=1; x < instr1.numTokens-1; x++)
		{
			if(strcmp(instr1.tokens[x], pi) != 0 && foundPipe == 0)
			{

			
				addToken(&instr2, instr1.tokens[x]);
			}
			if(strcmp(instr1.tokens[x], pi) == 0)
			{
				foundPipe = 1;
				afterpipespot = x+1;

			}
			if(x == afterpipespot)
			{
	
				final2 = findPath(instr1.tokens[x],path1);

				addToken(&instr3, final2);
			}
			if(foundPipe == 1 && strcmp(instr1.tokens[x], pi) != 0 && x > afterpipespot)
			{
				addToken(&instr3, instr1.tokens[x]);
				
			}
		}	

	addNull(&instr2);
	addNull(&instr3);

	singlePipe(&instr2, &instr3);

	clearInstruction(&instr2);
	clearInstruction(&instr3);
		
		
		
	}
	
	if(pipecount == 2 && (*instr.tokens[0] == '|'))
		printf("Error! Invalid pipe command!\n");
	else if(pipecount == 2)
	{
		char * final1 = (char*) malloc(size*sizeof(char));
		char * final2 = (char*) malloc(size*sizeof(char));
		char * final3 = (char*) malloc(size*sizeof(char));
		
		final1 = findPath(instr.tokens[0], path1);
		addToken(&instr4,final1);
		
		int foundPipe1 = 0;
		int foundPipe2 = 0;
		int p;
	//	char * pi = "|";
		int afterpipespot1 = -1;
		int afterpipespot2 = -1;
		int x;

		for(x=1; x<instr1.numTokens-1; x++)
		{
			if(*instr1.tokens[x] != '|' && foundPipe1 == 0)
                 addToken(&instr4, instr1.tokens[x]);
			if(*instr1.tokens[x] == '|' && foundPipe1 == 0)
			{
                foundPipe1 = 1;
				afterpipespot1 = x+1;
				p=0;
			}
			if(x == afterpipespot1)
			{
				final2 = findPath(instr1.tokens[x], path1);
				addToken(&instr5, final2);
				
			}
			if(foundPipe1 == 1 && foundPipe2 == 0 && x > afterpipespot1)
			{
				if(*instr1.tokens[x] != '|')
				{
					addToken(&instr5, instr1.tokens[x]);
					p++;
				}
						
			}
			if(foundPipe1 == 1 && *instr1.tokens[x] == '|' && p > 0)
			{
				foundPipe2 = 1;
				afterpipespot2 = x+1;
			}
			if(x == afterpipespot2)
			{
				final3 = findPath(instr1.tokens[x], path1);
				addToken(&instr6, final3);
			}

            if(foundPipe2 == 1 && *instr1.tokens[x] != '|' && x > afterpipespot2)      
				addToken(&instr6, instr1.tokens[x]);
                  
        }
		
		addNull(&instr4);
		addNull(&instr5);
		addNull(&instr6);
		doublePipe(&instr4,&instr5,&instr6);
		clearInstruction(&instr4);
		clearInstruction(&instr5);
		clearInstruction(&instr6);
		
	}
	clearInstruction(&instr1);
	clearInstruction(&instr);
		
		
    
    
//function implementations    
    
void singlePipe(instruction* instr2_ptr, instruction* instr3_ptr)
{
	
    int status;

	pid_t pid;
	pid_t pid2;
	int fd[2];
		
		
		
	pid = fork();
	if(pid==0) 
	{
		pipe(fd);
		pid2 = fork();
		if(pid2 == 0) 
		{
			close(1);
			dup(fd[1]);
			close(fd[0]);
			close(fd[1]);
			execv(instr2_ptr->tokens[0], instr2_ptr->tokens);
			printf("fail1");
				
		}
		else 
		{
			close(0);
			dup(fd[0]);
			close(fd[0]);
			close(fd[1]);
			execvp(instr3_ptr->tokens[0], instr3_ptr->tokens);
		}
	}
	else 
	{
		close(fd[0]);
		close(fd[1]);
		waitpid(pid2, &status, 0);
		waitpid(pid, &status, 0);
			
	}
	
}
void doublePipe(instruction* instr4_ptr, instruction* instr5_ptr, instruction* instr6_ptr)
{

		int status;
		pid_t pid;
		pid_t pid2;
		pid_t pid3;
		int fd[2];
		int fd2[2];
		
	
		pid = fork();

		if(pid == 0)
		{
	
			pipe(fd);
			pipe(fd2);
			pid2 = fork();
			if(pid2 == 0)
			{
		
				pid3 = fork();
				if(pid3 == 0)
				{
			
			
					close(0);
					dup(fd2[0]);
					close(fd[0]);
					close(fd[1]);
					close(fd2[0]);
					close(fd2[1]);
					execv(instr6_ptr->tokens[0], instr6_ptr->tokens);
					printf("Didnt work third\n");
				  
					
				}
				else
				{
				
				
					close(0);
					dup(fd[0]);
					close(1);
					dup(fd2[1]);
					close(fd[0]);
					close(fd[1]);
					close(fd2[0]);
					close(fd2[1]);
					execv(instr5_ptr->tokens[0], instr5_ptr->tokens);
					printf("Didnt work after\n");
					
				}
			}
			else
			{
	
					close(1);
					dup(fd[1]);
					close(fd[0]);
					close(fd[1]);
					close(fd2[0]);
					close(fd2[1]);
					execv(instr4_ptr->tokens[0], instr4_ptr->tokens); 
					printf("Didnt work before\n");
			}
		}
		else
		{
			close(fd[0]);
			close(fd[1]);
			close(fd2[0]);
			close(fd2[1]);
			waitpid(pid3, &status, 0);
			waitpid(pid2, &status, 0);
			waitpid(pid, &status, 0);
		}
			
			
    
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
