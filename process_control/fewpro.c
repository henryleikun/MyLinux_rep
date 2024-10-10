#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

void Runchild(){
  while(1){
   printf("I am child,pid:%d,ppid:%d\n",getpid(),getppid());
   sleep(1);
  }
}


int main(){
  
  for(int i = 0; i < 5; i++){
    pid_t id = fork();
    
    if(id == 0){//子进程去跑一下这个函数 然后退出
      Runchild();

      exit(0);
    }
  }//父进程才会走到这 父进程走了5次循环，创建5个子进程

  sleep(1000);//等着子进程都跑完那个函数

  return 0;
}
