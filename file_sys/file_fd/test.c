#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
  int fd = open("file.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);

  if(fd < 0){
    perror("Open error\n");
    return 1;
  }

  printf("stdin:%d\n",stdin->_fileno);
  printf("stdout:%d\n",stdout->_fileno);
  printf("stderr:%d\n",stderr->_fileno);

  //printf("fd:%d\n",fd);
  //const char* mes = "Hello Linux!\n";
  //write(1,mes,strlen(mes));
  
  //char buffer[1024];
  //read(0,buffer,sizeof(buffer));
  //printf("echo:%s\n",buffer);
 

  //FILE* fp = fopen("file.txt","w");
  //if(fp == NULL){
  //  perror("Open failed\n");
  //  return 1;
  //}
  //umask(0);
  //int fd = open("file.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
  //if(fd < 0){
  //  perror("Open failed\n");
  //  return 1;
  //}
  //
  //const char* message = "hello Linux\n";
  ////fwrite(message,1,strlen(message),fp);//不用strlen + 1
  ////fclose(fp);
  //write(fd,message,strlen(message));
  //close(fd);

  return 0;
}
