#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <cstdio>
using namespace std;

int g_val = 10;

string toHex(pthread_t tid){
    char hex[64];
    snprintf(hex,sizeof(hex),"%p",tid);
    return hex;
}

void show(const string& name){
    cout << name << "say " << "hello thread" << endl;
}

class Request{
private:
   int _start;
   int _end;
   string _threadname;
public:
   Request(int start = 0,int end = 0,const string& name = "new thread")
   :_start(start),_end(end),_threadname(name){

   }

   int Add(){
      int sum = 0;
      for(int i = _start; i <= _end; ++i) sum += i;

      return sum;
   }
};

class Response{
private:
   int _exitcode;//0表示结果正确，非零错误
   int _ret;
public:
   Response(int exit = 0,int ret = 0)
   :_ret(ret),_exitcode(exit){

   }

   int Getvalue()const{
      return _ret;
   }
};

void* ThreadRoutine(void* args){
     Request* preq = static_cast<Request*>(args);//安全的类型转换
     int sum = preq->Add();
     Response* pres = new Response(0,sum);
     
     delete preq;
     return (void*)pres;
}

int main(){
    pthread_t tid;
    Request* preq = new Request(1,100,"Thread 1 "); 
    pthread_create(&tid,nullptr,ThreadRoutine,(void*)preq);
    
    void* ret;
    pthread_join(tid,&ret);
    Response* pres = static_cast<Response*>(ret);
    cout << "Return Value:" << pres->Getvalue() << endl;
    delete pres;

    return 0;
}

// void* ThreadRoutine(void* args){
//     // const char* name = (const char*) args;
//     // cout << name << " say:" << "Hello tread" << endl;

//     // while(1){
//     //   //cout << "I am new thread,my pid:" << getpid() << ",&g_val:" << &g_val << "g_val: " << g_val << endl;
//     //   show("[new thread]");
//     //   sleep(1);
//     // }
//     //exit(3);

//     // int a = 10;
//     // a /= 0;

//     // cout << "before thread" << endl;

//     // sleep(5);

//     // cout << "after thread" << endl;
//     cout << "I am new thread,my tid:" << toHex(pthread_self()) << endl;
//     pthread_exit((void*)100);

//     return (void*) 1;
// }

// int main(){
//     pthread_t tid;
//     //pthread_create(&tid,nullptr,ThreadRoutine,nullptr);
//     void* ret;
//     pthread_create(&tid,nullptr,ThreadRoutine,(void*)"Thread 1");
    
//     // sleep(1);
//     // pthread_cancel(tid);
//     // sleep(1);
    
//     pthread_join(tid,&ret);
//     cout << "main thread quit,get another thread return value:" << (long long int)ret << endl;

//     // while(1){
//     //   //cout << "I am old thread,my pid:" << getpid() << ",&g_val:" << &g_val << "g_val: " << g_val << endl;
//     //   //show("[old thread]");
//     //   sleep(1);
//     //   //g_val++;
//     // }
    
//     return 0;
// }