#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
using namespace std;

void handler(int num){
    cout << "我是" << num << "号信号" << endl;
}

int main(){

  for(int i = 1; i < 32; i++){
    signal(i,handler);
  }

  while(1){
    cout <<  "I am a process,pid:" << getpid() << endl;
    sleep(1);
  }
   
 
  return 0;
}
