#include <stdio.h>
#include <unistd.h>

int main(){
  pid_t id = fork();//fork系统调用 给父线程返回子线程的pid 给子线程返回0
                 //当父线程里有多个子线程的时候便于区分和管理 每个子线程只有一个父线程
                 //fork()的实现里面就已经对子线程进行了创建 里面的return语句在不同线程执行了两次
                 //所以id在两个线程里面 是有不同的值 线程通过PCB这个描述对象来进行管理
  if(id < 0){
    perror("fork");
  }
  else if(id == 0){
    printf("I am a child process! pid:%d ppid:%d\n",getpid(),getppid());
  }
  else{
    printf("I am a parent process! pid:%d ppid:%d\n",getpid(),getppid());
  }

  return 0;
}
