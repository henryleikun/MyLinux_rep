#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
//  while(1){
//    printf("I am a process\n");
//    sleep(1);
//  }
   pid_t id = fork();

   if(id < 0){
     printf("创建子进程失败\n");
     return 0;
   }
   else if(id == 0){
     //子进程
     while(1){
       printf("I am a child process!\n");
       sleep(3);
       break;
     }

     exit(0);
   }
   else{
     while(1){  
      printf("I am a father process!\n");
      sleep(1);
     }
   }

  return 0;
}
