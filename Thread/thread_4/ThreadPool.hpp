#pragma once
#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unistd.h>
#include "Task.hpp"
using namespace std;

struct ThreadData{
   pthread_t tid;
   string threadname;
}; 

template<class T>
class ThreadPool{
private:
   vector<ThreadData> threads_;//保存线程信息
   queue<T> taskpool_;//任务池，也是临界资源
   int threadnum_;//线程的个数 任务没有个数 想push几个push几个 

   pthread_mutex_t mutex_;//维护任务池的锁 保证各线程间的互斥关系
   pthread_cond_t cond_;//维护各线程间的同步关系——>由任务池是否为空触发

   void Lock(){
      pthread_mutex_lock(&mutex_);
   }

   void Unlock(){
      pthread_mutex_unlock(&mutex_);
   }

   void Wakeup(){
      pthread_cond_signal(&cond_);
   }

   void Sleep(){
      pthread_cond_wait(&cond_,&mutex_);
   }

   string Getthreadname(pthread_t tid){
      for(auto& e : threads_){
        if(e.tid == tid)
           return e.threadname; 
      }

      return "None";
   }

   bool IsEmpty(){
      return taskpool_.empty();
   }
public:
   ThreadPool(int num = 5):threadnum_(num),threads_(num){
      pthread_mutex_init(&mutex_,nullptr);
      pthread_cond_init(&cond_,nullptr);
   }
   
   //线程执行函数不能是成员函数，因为成员函数都有一个默认参数this指针，这里搞成static把this指针消去
   //但这样又不能访问类里的普通成员和成员函数了——>只能访问静态成员和静态函数，所以这里把每个对象的this指针作为参数传过来
   //所以类里要封装一些函数供这个函数方便调用
   static void* CarryTask(void* args){//消费者函数
      ThreadPool* tp = static_cast<ThreadPool*>(args);
      string name = tp->Getthreadname(pthread_self());

      while(true){
        tp->Lock();
        while(tp->IsEmpty()){//为空就进条件变量里 while是防止 伪唤醒
           tp->Sleep();
        }

        //唤醒 或不为空
        T task;
        tp->Pop(&task);
        tp->Unlock();
        //这里为了便于测试 就直接用Task类的方法了，如果是其他类型 这里要再做处理
        task();
        cout << name << " get a task,and the result is:" << task.GetResult() << endl;
      }
   }

   void Start(){
      for(int i = 0; i < threadnum_; i++){
         threads_[i].threadname = "Thread——" + to_string(i);   
         pthread_create(&threads_[i].tid,nullptr,CarryTask,this);
      }
   }

   void Push(const T& task){
      Lock();
      taskpool_.push(task);
      Wakeup();//push完之后就可以唤醒线程来接收这个任务了
      Unlock();
   }

   void Pop(T* task){//这里不能上锁了，因为线程在执行函数在上锁期间也要执行Pop函数 避免死锁
      *task = taskpool_.front();
      taskpool_.pop();
   }
   

   ~ThreadPool(){
      pthread_mutex_destroy(&mutex_);
      pthread_cond_destroy(&cond_);
   }
};