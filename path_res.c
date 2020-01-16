#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int exists(const char *fname){
  if(access(fname, F_OK) != -1){
    // file exists
  }else {
    // file doesn't exist
  }
}
int main(){
  int size = 4;
  char *PATH_INPUT = (char *)malloc(sizeof(char)*size);

  scanf("%s", PATH_INPUT);
  printf("%s\n", getenv("PATH"));

  char * token = strtok(getenv("PATH"), ":");
  char * temp = (char *)malloc(sizeof(char)*size);
  // loop through to get all paths
  while(token != NULL){
    printf( " %s\n", token );
    //path concatenated with provided pathname
    strcpy(temp, token);
    strcat(temp, "/");
    strcat(temp, PATH_INPUT);
    printf(" %s\n", temp);
    exists(temp);
    strcpy(temp, "");
    token = strtok(NULL, ":");
  }
return 0;
}
