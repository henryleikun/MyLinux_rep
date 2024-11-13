#pragma once
#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unistd.h>
#include "Task.hpp"
using namespace std; 

template<class T>
class ThreadPool{
private:
   vector<pthread_t> threads_;//保存线程id
   queue<T> taskpool_;//任务池，也是临界资源
   int threadnum_;//线程的个数 任务没有个数 想push几个push几个 

   pthread_mutex_t mutex_;//维护任务池的锁 保证各线程间的互斥关系
   pthread_cond_t cond_;//维护各线程间的同步关系——>由任务池是否为空触发

   static ThreadPool<T>* tp_;//static确定单例模式，指针确定懒汉模式 静态变量只能在类外初始化
   static pthread_mutex_t poollock_;//保护单例的指针

private:
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

   bool IsEmpty(){
      return taskpool_.empty();
   }

public:
   static void* CarryTask(void* args){//消费者函数
      ThreadPool* tp = static_cast<ThreadPool*>(args);

      while(true){
        tp->Lock();
        while(tp->IsEmpty()){//为空就进条件变量里等待 while是防止 伪唤醒
           tp->Sleep();
        }

        //唤醒 或不为空
        T task;
        tp->Pop(&task);
        tp->Unlock();
        task();//对应线程进行服务
      }
   }

   static ThreadPool<T>* GetInstance(){
      if(nullptr == tp_){

         pthread_mutex_lock(&poollock_);
         if(nullptr == tp_){
            cout << "Create a threadpool singleton!" << endl;
            tp_ = new ThreadPool<T>();
         }
         pthread_mutex_unlock(&poollock_);
      
      }

      return tp_;
   }

   void Start(){
      for(int i = 0; i < threadnum_; i++){
         pthread_create(&threads_[i],nullptr,CarryTask,this);
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
   
private:
   ThreadPool(int num = 10):threadnum_(num),threads_(num){
      pthread_mutex_init(&mutex_,nullptr);
      pthread_cond_init(&cond_,nullptr);
   }

   ~ThreadPool(){
      pthread_mutex_destroy(&mutex_);
      pthread_cond_destroy(&cond_);
   }

   ThreadPool(const ThreadPool<T>& rhs) = delete;
   const ThreadPool<T>& operator=(const ThreadPool<T>& rhs) = delete;
};

template<class T>
ThreadPool<T>* ThreadPool<T>::tp_ = nullptr;
template<class T>
pthread_mutex_t ThreadPool<T>::poollock_ = PTHREAD_MUTEX_INITIALIZER;