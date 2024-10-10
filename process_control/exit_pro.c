#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


int main(){
  
  printf("Hello Linux");
  _exit(0);
     
  
  //char* ptr =(char*)malloc(1000*1000*1000*4);
  // if(ptr == NULL){
  //  printf("%d:%s\n",errno,strerror(errno));
  //  return 1;
  //}
  
  //double d = 1 / 0;
  //char* str = "hello";
  // *str = 'H';

  //for(int i = 0; i < 150; i++){
  //   printf("%d:%s\n",i,strerror(i));//我也不知道有几个退出码，就先看150个
  // }
  //printf("Hello Linux\n");
   
  return 0;
}
