#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int g_val = 100;

int main(){ 
  pid_t id = fork();

  if(id < 0){
    perror("创建子进程失败\n");
    return 1;
  }
  else if(id == 0){//child
   int cnt = 5;

    while(1){
      printf("I am child, pid=%d, ppid=%d, g_val=%d, &g_val=%p\n",getpid(),getppid(),g_val,&g_val);
      sleep(1);

      if(cnt)
        cnt--;
      else{//cnt == 0
        g_val = 200;
        cnt--;
      }
    }
  }
  else{//father
    while(1){
      printf("I am father, pid=%d, ppid=%d, g_val=%d, &g_val=%p\n",getpid(),getppid(),g_val,&g_val);
      sleep(1);
    }
  }

  return 0;
}
