#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char* argv[]){
  if(argc != 2){
    perror("至少一个选项\n");
    return 1;
  }

  if(strcmp(argv[1],"-a") == 0){
    printf("-a\n");
  }

  if(strcmp(argv[1],"-l") == 0){
    printf("-l\n");
  }
  
  return 0;
}
