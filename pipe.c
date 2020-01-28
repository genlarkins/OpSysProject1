#include <unistd.h>


void singlePipe(instruction * instr_ptr);
void doublePipe(instruction* instr_ptr);

void singlePipe(instruction* instr_ptr)
{
    int i;
	int firstPipe = 0;
	int numBeforeFpipe = 0;
	int numAfterFpipe = 0;
	int pipecount = 0;
    
    
    if(*(instr_ptr->tokens)[0] == '|' || *(instr_ptr->tokens)[instr_ptr->numTokens-2] == '|')
    {
        printf("Error! Invalid pipe command!\n");
        return;
    }

    
	for (i = 0; i < instr_ptr->numTokens; i++)
	{
		if ((instr_ptr->tokens)[i] != NULL)
		{
			if(*(instr_ptr->tokens)[i] == '|')
				pipecount++;
		}
	}
    
    char ** beforePipe;

    char ** afterPipe;

    
 //   char * temp;
    int size;
    int status;
	int foundPipeFlag = 0;
	int u;
	int x;

	for(u = 0; u<instr_ptr->numTokens; u++)
	{
   //     temp = (char*)calloc(100,(strlen((instr_ptr->tokens)[u]) + 1) * sizeof(char));
//		strcpy(holder, (instr_ptr->tokens)[u] );
    //    temp = (instr_ptr->tokens)[u];
   //     int size = strlen(temp);
     //   printf("Size: %d\n", size);
        
        if (u == 0)
        {
            beforePipe = (char**) calloc(100,sizeof(char*));
            beforePipe[u] = (char *)malloc((strlen((instr_ptr->tokens)[u])+1) * sizeof(char));
            strcpy(beforePipe[u], (instr_ptr->tokens)[u]);
            printf("Before Pipe #%d: %s\n", u, beforePipe[u]);
        }
        else if(*(instr_ptr->tokens)[u] != '|' && foundPipeFlag == 0)
        {
            beforePipe = (char**) realloc(beforePipe, (u+1) * sizeof(char*));
            
            beforePipe[u] = (char *)malloc((strlen((instr_ptr->tokens)[u])+1) * sizeof(char));
            strcpy(beforePipe[u], (instr_ptr->tokens)[u]);
            printf("Before Pipe #%d: %s\n", u, beforePipe[u]);
        }
        else if(*(instr_ptr->tokens)[u] == '|')
        {
            foundPipeFlag = 1;
			x = 0;
        }
        else if(foundPipeFlag == 1 && *(instr_ptr->tokens)[u] != '|')
        {
            if(x == 0)
            {
                afterPipe = (char**) calloc(100,sizeof(char*));
                afterPipe[x] = (char *)malloc((strlen((instr_ptr->tokens)[u])+1) * sizeof(char));
                strcpy(afterPipe[x], (instr_ptr->tokens)[u]);
                printf("After Pipe #%d: %s\n", x, afterPipe[x]);
            }
            else
            {
                afterPipe = (char**) realloc(afterPipe, (x+1) * sizeof(char*));
                afterPipe[x] = (char *)malloc((strlen((instr_ptr->tokens)[u])+1) * sizeof(char));
                strcpy(afterPipe[x], (instr_ptr->tokens)[u]);
                printf("After Pipe #%d: %s\n", x, afterPipe[x]);
                x++;	
            }
        }			
				
	}
		
       

	pid_t pid;
	pid_t pid2;
	int fd[2];
		
		
		
	pid = fork();
	printf("After first fork\n");
	if(pid==0) 
	{
		printf("In pid == 0\n");
		pipe(fd);
		printf("After pipe(FD)\n");
		pid2 = fork();
		if(pid2 == 0) 
		{
			printf("In pid2 == 0\n");
			close(1);
			dup(fd[1]);
			close(fd[0]);
			close(fd[1]);
			execvp(beforePipe[0], beforePipe);
				
		}
		else 
		{
			close(0);
			dup(fd[0]);
			close(fd[0]);
			close(fd[1]);
			execvp(afterPipe[0], afterPipe);
		}
	}
	else 
	{
		close(fd[0]);
		close(fd[1]);
		waitpid(pid2, &status, WNOHANG);
		waitpid(pid, &status, WNOHANG);
			
	}
	
}
void doublePipe(instruction* instr_ptr)
{
    
    if(*(instr_ptr->tokens)[0] == '|' || *(instr_ptr->tokens)[instr_ptr->numTokens-2] == '|')
    {
        printf("Error! Invalid pipe command!\n");
        return;
    }
     //   char * temp;
        char ** beforePipe;
        char ** afterPipe;
        char ** thirdPipe;
    	int foundFirstPipeFlag = 0;
		int foundSecondPipeFlag = 0;
		int u;
		int x;
		int p;
		for(u = 0; u<instr_ptr->numTokens; u++)
		{
            if (u == 0)
            {
                beforePipe = (char**) calloc(100,sizeof(char*));
                beforePipe[u] = (char *)malloc((strlen((instr_ptr->tokens)[u])+1) * sizeof(char));
                strcpy(beforePipe[u], (instr_ptr->tokens)[u]);
                printf("Before Pipe #%d: %s\n", u, beforePipe[u]);
            }
            
            
            
		//	strcpy(holder, (instr_ptr->tokens)[u] );
    //        temp = (char*)calloc(100,(strlen((instr_ptr->tokens)[u]) + 1) * sizeof(char));
	//		temp = (instr_ptr->tokens)[u];
			else if(*(instr_ptr->tokens)[u] != '|' && foundFirstPipeFlag == 0)
			{
                
                beforePipe = (char**) realloc(beforePipe, (u+1) * sizeof(char*));

                beforePipe[u] = (char *)malloc((strlen((instr_ptr->tokens)[u])+1) * sizeof(char));
                strcpy(beforePipe[u], (instr_ptr->tokens)[u]);
                printf("Before Pipe #%d: %s\n", u, beforePipe[u]);
                
                
		//		beforePipe[u] = temp;
		//		printf("Before Pipe #%d: %s\n", u, beforePipe[u]);	
			}
			else if(*(instr_ptr->tokens)[u] == '|' && foundFirstPipeFlag == 0)
			{
				foundFirstPipeFlag = 1;
				x = 0;
			}
			else if(*(instr_ptr->tokens)[u] == '|' && foundFirstPipeFlag == 1 && x != 0)
			{
				foundSecondPipeFlag = 1;
				p = 0;
			}
			
			
			else if(foundFirstPipeFlag == 1 && *(instr_ptr->tokens)[u] != '|' && foundSecondPipeFlag == 0)
			{
                if(x==0)
                {
                    afterPipe = (char**) calloc(100,sizeof(char*));
                    afterPipe[x] = (char *)malloc((strlen((instr_ptr->tokens)[x])+1) * sizeof(char));
                    strcpy(afterPipe[x], (instr_ptr->tokens)[u]);
                    printf("After Pipe #%d: %s\n", x, afterPipe[x]);
                }
                else
                {
                    afterPipe = (char**) realloc(afterPipe, (u+1) * sizeof(char*));

                    afterPipe[x] = (char *)malloc((strlen((instr_ptr->tokens)[x])+1) * sizeof(char));
                    strcpy(afterPipe[x], (instr_ptr->tokens)[u]);
                    printf("After Pipe #%d: %s\n", x, afterPipe[x]);
                }
	//			afterPipe[x] = temp;
	//			printf("After Pipe #%d: %s\n", x, afterPipe[x]);
				x++;	
			}
			if(foundSecondPipeFlag == 1 && *(instr_ptr->tokens)[u] != '|')
			{
                if(p == 0)
                {
                    thirdPipe = (char**) calloc(100,sizeof(char*));
                    thirdPipe[p] = (char *)malloc((strlen((instr_ptr->tokens)[p])+1) * sizeof(char));
                    strcpy(thirdPipe[p], (instr_ptr->tokens)[u]);
                    printf("Third Pipe #%d: %s\n", p, thirdPipe[p]);
                }
                else
                {
                    thirdPipe = (char**) realloc(afterPipe, (u+1) * sizeof(char*));

                    thirdPipe[p] = (char *)malloc((strlen((instr_ptr->tokens)[p])+1) * sizeof(char));
                    strcpy(thirdPipe[p], (instr_ptr->tokens)[u]);
                    printf("Third Pipe #%d: %s\n", p, thirdPipe[p]);
                }
		//		thirdPipe[p] = temp;
		//		printf("Third Pipe #%d: %s\n", p, thirdPipe[p]);
                p++;
			}
			
				
		}
		
		
		
		
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
					execvp(thirdPipe[0], thirdPipe);
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
					execvp(afterPipe[0], afterPipe);
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
					execvp(beforePipe[0], beforePipe); // im using execvp because that was what was done in the hints ppt so pls dont take off for this im begging you
					printf("Didnt work before\n");
			}
		}
		else
		{
			close(fd[0]);
			close(fd[1]);
			close(fd2[0]);
			close(fd2[1]);
			waitpid(pid3, &status, WNOHANG);
			waitpid(pid2, &status, WNOHANG);
			waitpid(pid, &status, WNOHANG);
		}
			
			
    
}
