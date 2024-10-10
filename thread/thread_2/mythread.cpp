#include <iostream>
#include <pthread.h>
#include <string>
#include <cstdio>
#include <vector>
#include <unistd.h>
#include <cstdlib>
#include <string.h>
#include "mythread.hpp"
using namespace std;
#define NUM  4

int tickets = 1000;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

string toHex(pthread_t tid){
    char buffer[64];
    snprintf(buffer,sizeof(buffer),"0x%x",tid);
    
    return buffer;
}

class ThreadData{
public:
   string threadname;
   //pthread_mutex_t* _lock;
public:
   // ThreadData(string name,int num,pthread_mutex_t* lock){
   //    threadname = name + to_string(num);      
   //    _lock = lock;
   // }

   ThreadData(string name,int num){
      threadname = name + to_string(num);      
   }

};

//抢票函数
void* GetTicket(void* args){
   ThreadData* thread = static_cast<ThreadData*>(args);

   while(1){
    
     {
       LockGuard locklock(&lock);

       if(tickets > 0){
         usleep(1000);
         printf("%s get a ticket,remaining tickets:%d\n",thread->threadname.c_str(),tickets);
         tickets--;
       }
       else 
         break;
      }//出作用域自动析构

      usleep(15);
   }

   std::cout << thread->threadname << " quit " << endl;
   return nullptr;
}

//抢票函数
// void* GetTicket(void* args){
//    ThreadData* thread = static_cast<ThreadData*>(args);

//    while(1){
//       //pthread_mutex_lock(thread->_lock);
//       pthread_mutex_lock(&lock);
//       if(tickets > 0){
//          usleep(1000);
//          printf("%s get a ticket,remaining tickets:%d\n",thread->threadname.c_str(),tickets);
//          tickets--;
//          //pthread_mutex_unlock(thread->_lock);
//          //pthread_mutex_unlock(&lock);
//       }
//       else {
//          //pthread_mutex_unlock(thread->_lock);
//          //pthread_mutex_unlock(&lock);
//          break;
//       }
//       usleep(15);
//    }

//    cout << thread->threadname << " quit " << endl;
//    return nullptr;
// }

int main(){
   //pthread_mutex_t lock;
   //一把锁就够了， 保证每个线程都看到这把锁
   //pthread_mutex_init(&lock,nullptr);

   vector<pthread_t> tids;
   vector<ThreadData* > thread_datas;

   for(int i = 1; i <= NUM; ++i){//创建的线程编号从1开始
      pthread_t tid;
      //ThreadData* thread = new ThreadData("Thread——",i,&lock);
      ThreadData* thread = new ThreadData("Thread——",i);
      thread_datas.push_back(thread);

      pthread_create(&tid,nullptr,GetTicket,thread);
      tids.push_back(tid);
   }

   for(int i = 0; i < NUM; ++i) pthread_join(tids[i],nullptr);
   for(auto& e : thread_datas) delete e;

   //pthread_mutex_destroy(&lock);

   return 0;
}

// string toHex(pthread_t tid){
//     char buffer[64];
//     snprintf(buffer,sizeof(buffer),"0x%x",tid);
    
//     return buffer;
// }

// class ThreadData{
// public:
//    string threadname;
// public:
//    ThreadData(string name,int num){
//       threadname = name + to_string(num);      
//    }

// };

// // int g_val = 100;
// //int* p;
// __thread int g_val = 100;

// void* ThreadRoutine(void* args){
//    ThreadData* thread = static_cast<ThreadData*>(args);    
//    string tid = toHex(pthread_self());

//    // int testi = 1;
//    // if(thread->threadname == "thread-2") p = &testi;
//    if(thread->threadname == "thread-2") pthread_detach(pthread_self());

//    int cnt = 5; 
//    while(cnt--){
//    //   cout << thread->threadname << "——>" << "pid: " << getpid() << ",tid: " << tid 
//    //   << ",g_val: " << g_val << ",&g_val: " << &g_val << endl;
//    printf("%s——>pid:%d,tid:%p\n",thread->threadname.c_str(),getpid(),pthread_self());
     
//    //g_val++;
//    sleep(1);  
//    }

//    delete thread;

//    return nullptr;
// }


// int main(){
//     vector<pthread_t> tids;

//     for(int i = 0; i < NUM; i++){//创建NUM个线程(带上主线程总线程是NUM+1个)
//        pthread_t tid;
       
//        //ThreadData thread("thread-",i);
//        //pthread_create(&tid,nullptr,ThreadRoutine,&thread); 不能这样写，thread的地址被另一个进程给引用
//        //但是，该thread是一个局部变量，出了for循环就释放了，对于那边的线程拿到的地址就是野指针了。
//        ThreadData* thread = new ThreadData("thread-",i);
//        pthread_create(&tid,nullptr,ThreadRoutine,thread);
//        tids.push_back(tid);
//     }
    
//    //  sleep(1);
//    //  cout << "main thread get the local value testi:" << *p << endl;
//    //  sleep(1);
//    //  for(auto& e : tids){
//    //     pthread_detach(e);
//    //  }

//     for(int i = 0; i < NUM; i++){
//       int n = pthread_join(tids[i],nullptr);//阻塞式等待
//       printf("n:%d,who:%p,why:%s\n",n,tids[i],strerror(n));
//     }

//     return 0;
// }