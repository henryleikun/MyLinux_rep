#pragma once
#include <iostream>
#include <pthread.h>
#include <ctime>
#include <string>
#include <functional>
#include <unistd.h>
using namespace std;

typedef function<void()> Func;//包装器 封装无参和无返回值的函数对象
int num = 1;

class Thread{
private:
   pthread_t tid_;
   string name_;
   Func threadrun;//线程执行函数
   bool isruning_;//是否在运行
   time_t runningtime_;//运行的时间戳
public:
   Thread() = default;
   
   Thread(Func f):tid_(0),threadrun(f),isruning_(false),runningtime_(0){
      
   }

   static void* ThreadRun(void* args){
      Thread* t = static_cast<Thread*>(args);
      string name = t->Getname();
      time_t time = t->Howtime();
      cout << name << " is running,and the timestamp is " <<  time << endl;
      t->Entry(); 

      return nullptr; 
   }

   void Run(){
      name_ = "Thread——" + to_string(num++);
      isruning_ = true;
      runningtime_ = time(nullptr);
      pthread_create(&tid_,nullptr,ThreadRun,this); 
   }

   string Getname()const{
      return name_;
   }

   time_t Howtime()const{
      return runningtime_;
   }

   bool IsRunning(){
      return isruning_;
   }

   void Entry(){
      threadrun();
   }

   void jojn(){
      pthread_join(tid_,nullptr);
   }
};