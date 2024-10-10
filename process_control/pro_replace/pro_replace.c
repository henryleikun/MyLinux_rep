#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
  extern char** environ;
  pid_t id = fork();

  if(id < 0){
    perror("创建子进程失败\n");
    return 1;
  }
  else if(id == 0){//child
    printf("before,I am child,pid:%d.ppid:%d\n",getpid(),getppid());
    //execle("/usr/bin/ls","ls","-a","-l",NULL,environ);
    //execl("./testcpp","testcpp",NULL);
    //execl("/usr/bin/bash","bash","test.sh",NULL);
    
    char* myargv[]={
      "testcpp",
      "-a",
      "-l",
      NULL
    };

    char* myenv[]={
      "Henry=3",
      "HAHA=1717",
      NULL
    };

    //execve("./testcpp",myargv,myenv);
    //execve("./testcpp",myargv,environ);
    execv("./testcpp",myargv);
    printf("after,I am child,pid:%d.ppid:%d\n",getpid(),getppid());

    exit(1);
  }
  else{//parent
    int status;

    pid_t ret = waitpid(id,&status,0);
    if(ret == id){
      printf("等待%d成功,退出码：%d\n",ret,WEXITSTATUS(status));
      sleep(1);
    }
    else{
     printf("等待失败\n");
    }

  }

 

 // printf("before,I am process,pid:%d.ppid:%d\n",getpid(),getppid());
 // execl("/usr/bin/ls","ls","-a","-l",NULL);
 // printf("after,pid:%d.ppid:%d\n",getpid(),getppid());

  return 0;
}
