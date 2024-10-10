#pragma once
#include <iostream>
#include <cstdio>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <unistd.h>

#define FIFO_PATH "./myfifo"
#define MODE 0666
#define SIZE 1024

enum {
    MKFIFO_ERROR = 1,
    UNLINK_ERROR,
    OPEN_ERROR,
 
};

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
