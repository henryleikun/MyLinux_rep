#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>

using namespace std;
int cnt = 0;
int NUM = 5;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* Rountine(void* args){
    pthread_detach(pthread_self());//分离自己释放自己
    u_int64_t number = (u_int64_t)args;
    cout << "thread——>" << number << "has created successfully" << endl; 

    while(true){
        pthread_mutex_lock(&mutex);//申请锁
        pthread_cond_wait(&cond,&mutex);
 
        cout << "thread——>" << number << ",cnt:" << cnt++ << endl;
        pthread_mutex_unlock(&mutex);
    }

    return  nullptr;
}

int main(){
    for(u_int64_t i = 0; i < NUM; i++){
        pthread_t tid;
                         
        pthread_create(&tid,nullptr,Rountine,(void*)i);//void*  是8B，int是4B这里的i就用uli了
        usleep(1000);
    }

    sleep(3);
    cout << "Main thread begin waking up threads..." << endl;
    
    while(true){
        //pthread_cond_signal(&cond);
        pthread_cond_broadcast(&cond);
        sleep(1);//每一秒唤醒一个线程
    }

    return 0;
}