#pragma once
#include <iostream>
#include <queue>
#include <pthread.h>
using namespace std;

template <class T>
class BlockQueue{
private:
   queue<T> blockqueue_;
   pthread_mutex_t mutex_;//需要一把锁来保护对应的阻塞队列——>临界资源
   int cap_;//阻塞队列的最大任务量(数据量)

   pthread_cond_t c_cond_;//生产者的条件变量(让生产者阻塞的时候在对应队列等待)
   pthread_cond_t p_cond_;//消费者的条件变量(让消费者阻塞的时候在对应队列等待)

   //int longwater_;//阻塞队列的任务量超过这个值才开始唤醒消费者
   //int lowwater_;//阻塞队列低于这个任务量才开始唤醒生产者
public:
    BlockQueue(int cap = 5):cap_(cap){
       pthread_mutex_init(&mutex_,nullptr);
       pthread_cond_init(&c_cond_,nullptr);
       pthread_cond_init(&p_cond_,nullptr);

    //    longwater_ = 3;
    //    lowwater_ = 2;
    }
  
    void Push(const T& task){//向阻塞队列里添加任务 生产者线程调
       pthread_mutex_lock(&mutex_);//先申请锁
       //if(blockqueue_.size() >= cap_)
       while(blockqueue_.size() >= cap_){//如果阻塞队列满了，那生产者就不能在生产，只能等消费者消费完来唤醒自己
          pthread_cond_wait(&p_cond_,&mutex_);
       }

       //没有满 或被唤醒
       blockqueue_.push(task);
       //刚放进去一个，消费者肯定能消费，这时候来唤醒一下对应的消费者
       pthread_cond_signal(&c_cond_);//唤醒一个
       //if(blockqueue_.size() > longwater_) pthread_cond_signal(&c_cond_);//消费者比较快
       pthread_mutex_unlock(&mutex_);//归还锁
    }

    void Pop(T* task){//从阻塞队列里拿任务——输出型参数 消费者线程调
       pthread_mutex_lock(&mutex_);
       //if(blockqueue_.empty())
       while(blockqueue_.empty()){//阻塞队列是空的就别拿了，去对应的条件变量里等待，等生产者唤醒
          pthread_cond_wait(&c_cond_,&mutex_);
       }

       //有东西  或被唤醒
       *task = blockqueue_.front();
       blockqueue_.pop();
       //被取走一个肯定有空位，可以唤醒生产者生产
       pthread_cond_signal(&p_cond_);//唤醒一个
       //if(blockqueue_.size() < lowwater_) pthread_cond_signal(&p_cond_);//生产者比较快
       pthread_mutex_unlock(&mutex_);//释放锁
    }
     
    ~BlockQueue(){
        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&c_cond_);
        pthread_cond_destroy(&p_cond_);
    }

};