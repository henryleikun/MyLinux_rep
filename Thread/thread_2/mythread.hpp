#pragma once
#include <pthread.h>

class LockGuard{
private:
   pthread_mutex_t* _lock;
public:
   LockGuard(pthread_mutex_t* lock = nullptr): _lock(lock) {
      pthread_mutex_lock(_lock);
   }

   ~LockGuard(){
      pthread_mutex_unlock(_lock);
   }
};