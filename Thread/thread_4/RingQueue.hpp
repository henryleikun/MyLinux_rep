#pragma once
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <vector>
using namespace std;

template<class T>
class RingQueue{
    void P(sem_t& SpaceSem){//对信号量资源做--
       sem_wait(&SpaceSem);
    }

    void V(sem_t& DataSem){//对信号量资源做++
       sem_post(&DataSem);
    }

    void Lock(pthread_mutex_t& mutex){
       pthread_mutex_lock(&mutex);
    }

    void Unlock(pthread_mutex_t& mutex){
       pthread_mutex_unlock(&mutex);
    }
private:
   vector<T> ringque_;//共享资源 环形队列

   //环形队列里的每个数据都是临界资源 本质上是要用锁来维护对应的下标 也就是下标是临界资源
   size_t pindex_;//生产者处在环形队列的下标
   size_t cindex_;//消费者处在唤醒队列的下标
   
   int cap_; //环形队列的最大容量
   //需要两把锁来维护两个下标 
   pthread_mutex_t pmutex_;//维护生产者之间的互斥关系
   pthread_mutex_t cmutex_;//维护消费者之间的互斥关系
   sem_t SpaceSem_; //空白资源信号量
   sem_t DataSem_; //数据资源的信号量
public:
   RingQueue(int cap = 10):cap_(cap),ringque_(cap),pindex_(0),cindex_(0){
      pthread_mutex_init(&pmutex_,nullptr);
      pthread_mutex_init(&cmutex_,nullptr);
      sem_init(&SpaceSem_,0,cap_);//初始为cap_
      sem_init(&DataSem_,0,0);//初始为0
   }

   void Push(const T& data){//生产者放进环形队列数据
      //这里注意好顺序，是先申请信号量还是先申请锁——都行，但是先申请信号量更好,原因如下
      //技术上:PV操作都是原子的，而之前说过，对应临界区里面的代码越少越好 原子性的PV操作跟加不加锁其实没关系
      //加锁也是申请，不加锁也是申请信号量
      //逻辑上:如果是先加锁再申请信号量，那一个线程访问到对应资源的时间是串行的，因为申请锁只能有一个拿到，剩下的
      //都要阻塞，之后再申请信号量就是串行相加,而如果是先申请信号量，不用阻塞，因为这是预定机制，你申请有就是有，
      //多个线程间申请信号量不用阻塞，这是并行时间相加
      P(SpaceSem_);
      Lock(pmutex_);
      ringque_[pindex_] = data;
      pindex_++;
      pindex_ %= cap_;
      Unlock(pmutex_);
      V(DataSem_);
   }

   void Pop(T* data){//消费者取出对应数据
      P(DataSem_);//申请信号量(空白)
      Lock(cmutex_);//申请锁
      *data = ringque_[cindex_];
      cindex_++;
      cindex_ %= cap_;
      Unlock(cmutex_);//释放锁
      V(SpaceSem_);//释放信号量(数据)
   }
   
   ~RingQueue(){
      pthread_mutex_destroy(&pmutex_);
      pthread_mutex_destroy(&cmutex_);
      sem_destroy(&SpaceSem_);
      sem_destroy(&DataSem_);
   }
};