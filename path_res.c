#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

//function to check existence of file
bool exists(const char *fname){
  if(access(fname, F_OK) != -1){
    printf("%s\n", "file exists");
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

int main(){
  int size = 4;
  char *PATH_INPUT = (char *)malloc(sizeof(char)*size);

  scanf("%s", PATH_INPUT);
  printf("%s\n", getenv("PATH"));

  char * token = strtok(getenv("PATH"), ":");
  //string for concatonated path names
  char * temp = (char *)malloc(sizeof(char)*size);
  //loop through to get all paths
  //if flag is still false, file is not executable
  bool file_flag = false;
  while(token != NULL){
    printf( " %s\n", token );
    //path concatenated with provided pathname
    strcpy(temp, token);
    strcat(temp, "/");
    strcat(temp, PATH_INPUT);
    printf(" %s\n", temp);
    if(exists(temp) && check_regular(temp)){
      strcpy(temp, "");
      file_flag = true;
      break;
    }
    strcpy(temp, "");
    token = strtok(NULL, ":");
  }

  if(!file_flag)
    printf("%s\n", "File does not exist or is not regular");

return 0;
}
