#include <stdio.h>
#include <unistd.h>

int main(){
  pid_t id = fork();
  int cnt = 5;

  if(id < 0){
    perror("创建子进程失败\n");
    return 1;
  }
  else if(id == 0 ){
    //child
    while(1){
      printf("I am a child process!\n");
      sleep(1);
    }
  }
  else{
    while(cnt--){
      printf("I am a father process!\n");
      sleep(1);
    }//运行5s后退出
  }

  return 0;
}
