#include <iostream>
#include <pthread.h>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include "BlockQueue.hpp"
#include "Task.hpp"

//生产者函数
void* Productor(void* args){
   BlockQueue<Task>* blockque = static_cast<BlockQueue<Task>*>(args);

   while(true){
      //sleep(1);
      
      //准备数据
      int data1 = rand() % 10 + 1;
      //blockque->Push(data);
      usleep(10);//让两个数更随机一点
      int data2 = rand() % 10 + 1;
      char op = opers[rand() % opers.size()];
      Task task(data1,data2,op);
      
      //放入数据
      blockque->Push(task);
      //cout << "Productor has created a task——" << data << endl;
      cout << "Productor——>" << pthread_self() << " create a task:" << task.GetTask() << endl;
      sleep(1);
   }
}

//消费者函数
void* Consumer(void* args){
   BlockQueue<Task>* blockque = static_cast<BlockQueue<Task>*>(args);

   while(true){
      //int data;
      //blockque->Pop(&data);
      //cout << "Consumer get a task——" << data << endl;
      
      Task task;
      blockque->Pop(&task);
      //处理数据
      task.Run();
      cout << "Consunmer——>" << pthread_self() << "get a task,and the result is" << task.GetResult() << endl;
      
      //sleep(1);
   }
}

int main(){
    srand(time(nullptr));
    // pthread_t c;//生产者线程
    // pthread_t p;//消费者线程
    pthread_t c[3];//生产者线程
    pthread_t p[5];//消费者线程
    
    //BlockQueue<int>* blockque = new BlockQueue<int>();
    BlockQueue<Task>* blockque = new BlockQueue<Task>();

    // pthread_create(&c,nullptr,Consumer,blockque);
    // pthread_create(&p,nullptr,Productor,blockque);
    for(int i = 0; i < 3; i++){
        pthread_create(c+i,nullptr,Consumer,blockque);
    }

    for(int i = 0; i < 5; i++){
        pthread_create(p+i,nullptr,Productor,blockque);
    }


    // pthread_join(c,nullptr);
    // pthread_join(p,nullptr);
    for(int i = 0; i < 3; i++){
        pthread_join(c[i],nullptr);
    }

    for(int i = 0; i < 5; i++){
        pthread_join(p[i],nullptr);
    }

    delete blockque;

    return 0;
}