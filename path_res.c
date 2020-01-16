#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
  int size = 4;
  char *PATH_INPUT = (char *)malloc(sizeof(char)*size);

  scanf("%s", PATH_INPUT);
  printf("%s\n", getenv("PATH"));

  char * token = strtok(getenv("PATH"), ":");
  // loop through to get all paths
  while(token != NULL){
    printf( " %s\n", token );
    token = strtok(NULL, ":");
  }
return 0;
}
