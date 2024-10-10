#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
  
  //close(1);
  int fd = open("file.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
  if(fd < 0){
    perror("Open error\n");
    return 1;
  }

  dup2(fd,1);
  close(fd);

  //printf("fd:%d\n",fd);
  const char* mes = "Hello Linux\n";
  write(1,mes,strlen(mes));
    
  return 0;
}
