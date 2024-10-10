#include "mystdio.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>


#define MODE 0666

_FILE* _fopen(const char* filename,const char* flag){
  assert(filename);
  assert(flag);
  
  int f = 0;
  int fd = -1;

  //我们只考虑三种情况
  if(strcmp(flag,"r") == 0){
    f = O_RDONLY;
    fd = open(filename,f,MODE);
  }
  else if(strcmp(flag,"w") == 0){
    f = O_WRONLY|O_CREAT|O_TRUNC;
    fd = open(filename,f,MODE);
  }
  else if(strcmp(flag,"a") == 0){
   f = O_WRONLY|O_CREAT|O_APPEND;
   fd = open(filename,f,MODE);
  }

  if(fd == -1) return NULL;

  _FILE* fp =(_FILE*)malloc(sizeof(_FILE));
  if(fp == NULL) return NULL;
  
  fp->fileno = fd;
  fp->flag = FLUSH_LINE;//默认行缓冲
  fp->out_pos = 0;
   
  return fp;
}

int _fwrite(_FILE* fp,const char* message,int size){
  //先把对应信息写到用户缓冲区
  memcpy(&fp->outbuffer[fp->out_pos],message,size);//这里不是很严谨，但无所谓了
  fp->out_pos += size;

  if(fp->flag & FLUSH_NOW){//直接缓冲
    write(fp->fileno,fp->outbuffer,fp->out_pos);
    fp->out_pos = 0;
  }
  else if(fp->flag & FLUSH_LINE){//行缓冲
    if(fp->outbuffer[fp->out_pos - 1] == '\n'){//这里就只考虑最后出现换行符的情况，中间出现就不写了
     //遍历字符串，中间出现\n就写入对应长度的信息，再把后边的信息提前
     write(fp->fileno,fp->outbuffer,fp->out_pos);
     fp->out_pos = 0;
    }
    else {
      //do nothing
    } 
  }
  else if(fp->flag & FLUSH_ALL){//全缓冲
    if(fp->out_pos >= SIZE){
       write(fp->fileno,fp->outbuffer,fp->out_pos);
       fp->out_pos = 0;
    }
  }

  return size;
}

void _fflush(_FILE* fp){
  if(fp->out_pos > 0){
    write(fp->flag,fp->outbuffer,fp->out_pos);
    fp->out_pos = 0;
  }

}

void _fclose(_FILE* fp){
   assert(fp);

  _fflush(fp);
  close(fp->fileno);
  free(fp);
}
