#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define TASK_NUM 10
typedef void (* task_t)();
task_t tasks[TASK_NUM];

void Runchild(){
  int cnt = 2;

  while(cnt){
    printf("I am child,pid:%d,ppid:%d,cnt=%d\n",getpid(),getppid(),cnt);
    cnt--;
    sleep(1);
  }
}


void task_1(){
  printf("执行任务1中\n");
}


void task_2(){
  printf("执行任务2中\n");
}

void task_3(){
  printf("执行任务3中\n");
}

void Add_task(task_t t){
  int pos = 0;

  for(; pos< TASK_NUM; pos++){
    if(tasks[pos] == NULL)
      break;
  }
  
  if(pos == TASK_NUM)
    return ;

  tasks[pos] = t;
}

void Init_task(){
  for(int i = 0; i < TASK_NUM; i++){
    tasks[i] = NULL;
  }

  Add_task(task_1);
  Add_task(task_2);
  Add_task(task_3);
}

void Execute_task(){
  for(int i = 0; i < TASK_NUM; i++){
    if(tasks[i])
      tasks[i]();
  }
}

int main(){
// for(int i = 0; i < 10; i++){
//    pid_t id  = fork();
//   
//    if(id == 0){//child
//     // Runchild();
//      int cnt = 3;
//      while(cnt){
//       printf("I am child,pid:%d,ppid:%d,%d\n",getpid(),getppid(),i);
//       sleep(1);
//       cnt--;
//      }
//      exit(0);
//   }
//  }
//  
//  sleep(5);
//
//  for(int i = 0; i < 10; i++){
//   pid_t ret = wait(NULL);
//
//   printf("等待%d成功\n",ret);
//  }



  pid_t id = fork();

  if(id < 0){
    printf("%s",strerror(errno));
    return 1;
  }
  else if(id == 0){//child
    Runchild();
    exit(3);
  }
  else{//parent
   int status;
   Init_task();
   
   while(1){
    pid_t ret = waitpid(-1,&status,WNOHANG);
    
    if(ret > 0){//等待成功
      if(WIFEXITED(status)){//没异常
       printf("等待%d成功,退出码:%d\n",ret,WEXITSTATUS(status));
       sleep(1);
      }
      else
        perror("子进程异常");

      break;
    }
    else if(ret <  0){
      perror("等待失败\n");
      break;
    }
    else{
     // printf("进程还未终止\n");
     // sleep(1);
     
     Execute_task();
     sleep(1);//微秒
    }

  }

 }

  return 0;
}
