#ifndef _MYSTDIO_H_
#define _MYSTDIO_H_ 

#define SIZE 1024
#define FLUSH_NOW 1
#define FLUSH_LINE 2
#define FLUSH_ALL 4

typedef struct _FILE_STRUCT{
  int fileno;//文件修饰符
  int flag;//确定缓冲区刷新方式
  //int in_pos;
  //char inbuffer[SIZE];
  //这里就搞一个输出缓冲区吧，主要是了解原理
  char outbuffer[SIZE];
  int out_pos;//记录缓冲区有多少个字符了
}_FILE;

_FILE* _fopen(const char* filename,const char* flag);
int _fwrite(_FILE* fp,const char* message,int size);
void _fflush(_FILE* fp);
void _fclose(_FILE* fp);


#endif
