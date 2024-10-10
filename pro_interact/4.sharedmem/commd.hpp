#pragma  once
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include "log.hpp"
#include <cerrno>
#include <cstring>

#define SSIZE 4096
#define pathname "/home/Henry/pro_interact"
#define proj_id  0666

#define FIFO_PATH "./myfifo"
#define MODE 0666

enum {
    MKFIFO_ERROR = 1,
    UNLINK_ERROR,
    OPEN_ERROR,
};

Log log;

key_t GetKey(){
   key_t key = ftok(pathname,proj_id);

   if(key == -1){
    log(Fatal,"ftok error:%s",strerror(errno));
    exit(1);
   }
   
   log(Info,"Get key done:0x%x",key);
   return key;
}

int ShmHelper(int flag){
   int shmid = shmget(GetKey(),SSIZE,flag);
   if(shmid == -1){
    log(Fatal,"shmget error:%s",strerror(errno));
    exit(1);
   }

   log(Info,"Creat shared memory done!\n");
   return shmid;    
}

int CreatShm(){//创建
   return ShmHelper(IPC_CREAT | IPC_EXCL | 0666);
}

int GetShm(){//获取
   return ShmHelper(IPC_CREAT);
}

class Init{
public:
    Init(){
      int r = mkfifo(FIFO_PATH,MODE);
      if(r < 0){
          //printf("mkfifo:%s",strerror(errno));
          perror("mkfifo");
          exit(MKFIFO_ERROR);
       }
    }

    ~Init(){
      int n = unlink(FIFO_PATH); 
      if(n < 0){
          perror("unlink");
          exit(UNLINK_ERROR);
      }
    }
};




