#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

//主执行程序在处理一个信号的时候，可以接受其他不同的信号——不同执行流，但正在执行的处理的信号会暂时被屏蔽
//在处理期间被屏蔽但是接收到的也就是未决的信号都会被保存，当处理结束，解除了屏蔽就继续执行
//并且当一个信号正在被处理时，我们发送了另一个没有被屏蔽的信号，那会紧接着去处理这个新信号，处理完后，在接着上一个信号处理
//一个进程，多个执行流
// void handler_2(int signo){
//     int cnt = 5;
//     while(cnt--){
//      cout << "Catch a signal number: " << signo << endl;
//      sleep(1);
//     }
// }

// void handler_3(int signo){
//     int cnt = 5;
//     while(cnt--){
//      cout << "Catch a signal number: " << signo << endl;
//      sleep(1);
//     }
// }

// int main(){
//     signal(2,handler_2);
//     signal(3,handler_3);

//     while(1){
//         cout << "I am a process,pid:" << getpid() << endl;
//         sleep(1);
//     }
 

//     return 0;
// }


void Print(sigset_t& pending){
   for(int i = 1; i <= 31; i++){
    if(sigismember(&pending,i))
      cout << 1;
    else
      cout << 0;
   }

   cout << endl << endl;
}

void handler(int signo){
    cout << "Catch a signumber:" << signo << endl;
    int cnt = 5;
    while(cnt--){
        sigset_t pending;
        sigpending(&pending);
        cout << "cnt = " << cnt << "---处理" << signo << "号信号中:"; 
        Print(pending);
        sleep(1); 
    }
}


// int main(){
//     struct sigaction act,oact;
//     memset(&act,0,sizeof(act));
//     memset(&oact,0,sizeof(oact));

//     sigaddset(&(act.sa_mask),1);
//     //sigaddset(&(act.sa_mask),3);
//     sigaddset(&(act.sa_mask),4);//在处理2的时候屏蔽1 3 4信号
//     act.sa_handler = handler;//也可以赋SIG_IGN,SIG_DFL
    
//     sigaction(2,&act,&oact);
//     signal(1,handler);
//     signal(3,handler);
//     signal(4,handler);

//     while(1){
//         cout << "I am a process,pid:" << getpid() << endl;
//         sigset_t pending;
//         sigpending(&pending); 
//         cout << "正常执行中:";
//         Print(pending);
//         sleep(1);
//     }

//     return 0;
// }


// volatile int flag = 0;

// void handler_flag(int signo){
//    cout << "Catch a signumber:" << signo << endl;
//    flag = 1;
// }

// int main(){
//     signal(2,handler_flag);
//     while(!flag) ;
 
//     cout << "over" << endl;
//     return 0;
// }

void handler_child(int signo){
    cout << "Catch a signumber:" << signo << endl;
    // for(int i = 0; i < 10; i++){
    //     int rid = waitpid(-1,nullptr,0);//-1接收任意一个子子进程
    //     if(rid < 0) exit(1);
    //     else if(rid > 0) cout << "等待成功" << endl;
    //     //if(rid > 0) cout << "等待成功" << rid << endl;
    // }

    pid_t rid;
    while((rid = waitpid(-1,nullptr,WNOHANG)) > 0){//非阻塞 等待无果返回零 继续会主执行流
        cout << "I am proccess: " << getpid() << " catch a signo: " << 
        signo << "child process quit: " << rid << endl;
    }
}

int main(){
    // signal(17,handler_child);
    signal(17,SIG_IGN);

    for(int i = 0; i < 10; i++){
        pid_t id = fork();

        if(id == 0){
          cout << " I am child process,pid:" <<getpid() << ",ppid:" << getppid() << endl;
          sleep(5);
          cout << "child quit" << endl;
          exit(0);
       }

       sleep(1);
    }

    //father
    while(1){
        cout << "I am father process,pid:" << getpid() << endl;
        sleep(1);
    }


    return 0;
}