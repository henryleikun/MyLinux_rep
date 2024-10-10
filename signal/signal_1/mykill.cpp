#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/wait.h>
using namespace std;

void handler(int num){
    cout << "我是" << num << "号信号" << endl;
    int n = alarm(50);
    cout << "还剩" << n << "秒" << endl;
}

// int main(){
//     for(int i = 1; i < 32; i++){
//       signal(i,handler);
//     }
//     while(1){
//       cout << "I am a process,pid:" << getpid() << endl;
//       sleep(1);
//     }
 
//     return 0;
// }

void Usage(){ 
    cout << "Usage\n\t";
    cout << "./mykill 信号编号  pid" << endl;
}


// int main(int argc,char* argv[]){
//     if(argc != 3){
//         Usage();
//         exit(1);
//     }

//     int signum = stoi(argv[1]);
//     pid_t pid = stoi(argv[2]);

//     int n = kill(pid,signum);
//     if(n == -1){
//         perror("kill");
//         exit(2);
//     }

//     return 0;
// }

// int main(){
//     int cnt = 3;
//     signal(6,handler);
//     while(cnt--){
//        cout << "I am a process,pid:" << getpid() << endl;
//        sleep(1);
//     }

//     //raise(2);
//     abort();

//     return 0;
// }

int main(){
    // int a = 10;
    // a /= 0;
    // signal(11,handler);

    // int* p = nullptr;
    // *p = 100;
    //signal(14,handler);
    // alarm(50);
    // handler(14);

    // while(1){
    //     cout << "I am a process,pid:" << getpid() << endl;
    //     sleep(1);
    // }
    
    pid_t id = fork();

    if(id == 0){
      int a = 10;
      a /= 0;
      
      exit(1);
    }
    
    int status = 0;
    int rid = waitpid(id,&status,0);

    if(rid < 0) exit(2);

    cout << "child quit info, rid: " << rid << " exit code: " << 
         ((status>>8)&0xFF) << " exit signal: " << (status&0x7F) << " core dump: " << ((status>>7)&1) << endl;
   
    return 0;
}