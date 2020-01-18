#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

//function to check existence of file
bool exists(const char *fname){
  if(access(fname, F_OK) != -1){
    //printf("%s\n", "file exists");
    return true;
  }
  else {
    return false;
  }
}

//function to check if file is regular
bool check_regular(const char *fname){
  struct stat file_stat;
  stat(fname, &file_stat);
  return S_ISREG(file_stat.st_mode);
}

//function to execute commands
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

int main(){
  int size = 4;
  char *PATH_INPUT = (char *)malloc(sizeof(char)*size);

  scanf("%s", PATH_INPUT);
  //printf("%s\n", getenv("PATH"));

  char * token = strtok(getenv("PATH"), ":");
  //string for concatonated path names
  char * concat = (char *)malloc(sizeof(char)*size);
  //string containing command + args
  char ** cmd = (char **)malloc(sizeof(char)*size);
  //loop through to get all paths
  //if flag is still false, file is not executable
  bool file_flag = false;
  while(token != NULL){
    //printf( " %s\n", token );
    //path concatenated with provided pathname
    strcpy(concat, token);
    strcat(concat, "/");
    strcat(concat, PATH_INPUT);
    //printf(" %s\n", concat);
    if(exists(concat) && check_regular(concat)){
      //strcpy(concat, "");
      file_flag = true;
      break;
    }
    strcpy(concat, "");
    token = strtok(NULL, ":");
  }

  if(!file_flag)
    printf("%s\n", "File does not exist or is not regular");
  else {
    cmd[0] = concat;
    my_execute(cmd);
  }

return 0;
}
