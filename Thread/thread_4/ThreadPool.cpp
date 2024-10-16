#include <iostream>
#include <pthread.h>
#include "ThreadPool.hpp"
#include "Task.hpp"

using namespace std;

int main(){
    ThreadPool<Task>* tp = new ThreadPool<Task>(5);
    tp->Start();

    while(true){
        //生产任务
        int data1 = rand() % 10 + 1;
        usleep(10);//让两个数更随机一点
        int data2 = rand() % 10 + 1;
        char op = opers[rand() % opers.size()];
        Task task(data1,data2,op);

        //push
        tp->Push(task);
        cout << "Main thread create a task" << task.GetTask() << endl;
        sleep(1);//生产的慢一点
    }
    
    delete tp;
    return 0;
}
