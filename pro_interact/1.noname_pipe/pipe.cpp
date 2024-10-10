#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <string>
using namespace std;

#define SIZE 1024
#define N 2

void Write(int fd){
   //int cnt = 5;
   string s = "Hello,I am child";
   char buffer[SIZE] = "Hello Linux\n";
   int number = 0; 

   while(1){
    buffer[0] = 0;//0是'\0'这里每次循环都清空一下
    snprintf(buffer,sizeof(buffer),"%s-%d-%d",s.c_str(),getpid(),number);
    write(fd,buffer,strlen(buffer));

    // char c = 'c';
    // write(fd,&c,sizeof(c));
    number++;
    //cout << number << endl;

    //sleep(1);//每隔一秒写一句
   }
}

void Read(int fd){
   char buffer[SIZE];
   int cnt = 3;

   while(cnt--){
    buffer[0] = 0;
    sleep(1);

    int ret = read(fd,buffer,sizeof(buffer));//注意这里是sizeof上面是strlen
    
    if(ret > 0){
      cout << "father get a message:" << buffer << endl;
    }
    else if(ret == 0){
        cout << "file is empty" << endl;
        break;
    }
    else //<0
       break;
   }
}

int main(){
    int pipefd[N] = {0};
    int n = pipe(pipefd);

    if(n < 0)
      return 1;

    pid_t id = fork(); 
    if(id < 0){
        return 1;
    }
    else if(id == 0){//child
        close(pipefd[0]);

        Write(pipefd[1]);
        exit(0);
    }
    
    //father
    close(pipefd[1]);
    Read(pipefd[0]);
    close(pipefd[0]);
    
    int status = 0;
    pid_t ret = waitpid(id,&status,0);
    if(ret < 0)
      return 1;
    
    cout << "异常信号：" << (status&0x7F) << endl;  
    

    // if(WIFEXITED(status)){//这是看退出码的，异常没意义
    //   printf()
    // }

    sleep(3);
    
    return 0;
}