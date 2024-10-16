#include <iostream>
#include <pthread.h>
#include <string>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include "RingQueue.hpp"
#include "Task.hpp"
using namespace std;

struct ThreadData{
   RingQueue<Task>* rq;
   string threadname;
};

//生产者函数
void* Productor(void* args){
   ThreadData* td = static_cast<ThreadData*>(args);
   RingQueue<Task>* rq = td->rq;
   string name = td->threadname;
   
   //生产
   while(true){
      //sleep(1);
      // int data = rand() % 10 + 1;
      // rq->Push(data);
      // cout << "Productor create data:" << data << endl;
      //准备数据
      int data1 = rand() % 10 + 1;
      usleep(10);//让两个数更随机一点
      int data2 = rand() % 10 + 1;
      char op = opers[rand() % opers.size()];
      Task task(data1,data2,op);
      
      //放入数据
      rq->Push(task);
      cout << name << " create a task:" << task.GetTask() << endl;
      sleep(1);
   }

   delete td;
   return nullptr;
}

//消费者函数
void* Consumer(void* args){
   ThreadData* td = static_cast<ThreadData*>(args);
   RingQueue<Task>* rq = td->rq;
   string name = td->threadname;

   //消费
   while(true){
      // sleep(1);
      // int data;
      // rq->Pop(&data);
      // cout << "Consumer get data:" << data << endl; 
      //sleep(1);
      Task task;
      rq->Pop(&task);

      //处理数据
      task.Run();
      cout << name << " get a task,and the result is " << task.GetResult() << endl;
   }
   
   delete td;
   return  nullptr;
}

int main(){
    srand(time(nullptr));
   //  pthread_t p;
   //  pthread_t c;
    pthread_t p[3];
    pthread_t c[5];
    RingQueue<Task>* rq = new RingQueue<Task>();

    //创建生产者消费者线程
   //  ThreadData* td1 = new ThreadData();
   //  td1->rq = rq;
   //  td1->threadname = "Productor";
   //  ThreadData* td2 = new ThreadData();
   //  td2->rq = rq;
   //  td2->threadname = "Consumer";
   //  pthread_create(&p,nullptr,Productor,td1);
   //  pthread_create(&c,nullptr,Consumer,td2);
   for(int i = 0; i < 3; i++){
        ThreadData* td = new ThreadData();
        td->rq = rq;
        td->threadname = "Productor——" + to_string(i);
        pthread_create(p+i,nullptr,Productor,td);
    }

    for(int i = 0; i < 5; i++){
        ThreadData* td = new ThreadData();
        td->rq = rq;
        td->threadname = "Consumer——" + to_string(i);
        pthread_create(c+i,nullptr,Consumer,td);
    }
    

    
    //销毁线程
   //  pthread_join(p,nullptr);
   //  pthread_join(c,nullptr);
   for(int i = 0; i < 3; i++){
        pthread_join(c[i],nullptr);
    }

    for(int i = 0; i < 5; i++){
        pthread_join(p[i],nullptr);
    }

    delete rq;

    return 0;
}