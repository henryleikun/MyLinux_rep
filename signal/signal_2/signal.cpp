#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

void handler(int signo){
    cout << "catch a signal number:" << signo << endl;
}

void Print(sigset_t& pending){
   for(int i = 1; i <= 31; i++){
    if(sigismember(&pending,i))
      cout << 1;
    else
      cout << 0;
   }

   cout << endl << endl;
}

int main(){
    //signal(2,SIG_IGN);
    // signal(2,SIG_DFL);

    // signal(2,handler);
    // sigset_t bset,oset;
    // sigemptyset(&bset);
    // sigemptyset(&oset);

    // sigaddset(&bset,2);
    // //将对应的位图设置到block里
    // sigprocmask(SIG_SETMASK,&bset,&oset);//屏蔽了2号信号
    
    // int cnt = 0;
    // sigset_t pending;
    // while(1){
    //   cout << "I am a process,pid:" << getpid() << endl;
    //   cnt++;
    //   sigpending(&pending);

    //   Print(pending);
    //   sleep(1);

    //   if(cnt == 10)
    //     sigprocmask(SIG_SETMASK,&oset,nullptr);//接触对二号信号的屏蔽       
    // }

    sigset_t bset,oset;
    sigemptyset(&bset);
    sigemptyset(&oset);

    for(int i = 1; i <= 31; i++){
        sigaddset(&bset,i);
    }

    //屏蔽所有信号
    sigprocmask(SIG_SETMASK,&bset,&oset);
    
    while(1){
        cout << "I am a process,pid:" << getpid() << endl;
        sleep(1);
        sigset_t pending;
        sigpending(&pending);
        Print(pending);
    }
    
    return 0;
}