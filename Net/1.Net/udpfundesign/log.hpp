#pragma once
#include <iostream>
#include <string>
#include <map>
#include <cstdarg>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <unistd.h>

using std::string;
using std::cout;
using std::endl;
using std::map;

#define SIZE 1024
#define Info 1
#define Warning 2
#define Error 3
#define Fatal 4
#define Debug 5

#define Screen 1
#define File 2
#define ClassFile 3

#define Filename "log.txt"

map<int, string> Level = {{1, "Info"},{2, "Warning"},{3, "Error"},{4, "Fatal"},{5, "Debug"}};

class Log
{
private:
    int print_method;
    string path;

public:
    Log(){
        print_method = Screen;//默认打印到屏幕上
        path = "./log/";//我们把生成的日志文件，放到一个目录里
    }

    void ModifyPrint(int method){
        print_method = method;
    }

    // void Logmessage(int level, const char *format, ...) {// level确定打印的等级，format是对应格式化内容
    //     char defaultbuffer[SIZE]; // 存日志等级和日志时间
    //     char custombuffer[SIZE];  // 自定义内容

    //     // 等级和时间的写入
    //     time_t t = time(nullptr);
    //     struct tm *ptm = localtime(&t);

    //     snprintf(defaultbuffer, sizeof(defaultbuffer), "[%s][%d-%d-%d-%d-%d-%d]:", Level[level].c_str(), ptm->tm_year + 1900,
    //              ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    //     // 自定义内容输入
    //     va_list s;
    //     va_start(s, format);

    //     // 我们这里就不对这个format做字符串解析了，我们直接调用对应的函数帮我们做这一步工作
    //     vsnprintf(custombuffer, sizeof(custombuffer), format, s);

    //     char logtxt[SIZE * 2];
    //     snprintf(logtxt, sizeof(logtxt), "%s %s", defaultbuffer, custombuffer);

    //     PrintLog(level, logtxt);
    // }

                             //其实这里要用右值引用，但const 左值引用也可以接受右值
    void PrintLog(int level, const string& log){ // 打印日志，分为打屏幕上，打文件里，分类打印文件
        switch(print_method){
           case Screen: 
             cout << log << endl;
           break;
           case File: 
             PrintOnefile(Filename,log);
           break;
           case ClassFile: 
             PrintClassFile(level,log);//level用来作为生成文件的后缀 
           break;
           default: break;
        }
    }

    void PrintOnefile(const string& filename,const string& log){
       string dir_filename = path + filename;
       int fd = open(dir_filename.c_str(),O_WRONLY|O_CREAT|O_APPEND,0666);
       if(fd < 0) exit(1);

       write(fd,log.c_str(),log.size());
       close(fd);
    }

    void PrintClassFile(int level,const string& log){
       string filename = Filename;
       filename += ".";
       filename += Level[level];

       PrintOnefile(filename,log);
    }

    void operator()(int level, const char *format, ...){
        char defaultbuffer[SIZE]; // 存日志等级和日志时间
        char custombuffer[SIZE];  // 自定义内容

        // 等级和时间的写入
        time_t t = time(nullptr);
        struct tm *ptm = localtime(&t);

        snprintf(defaultbuffer, sizeof(defaultbuffer), "[%s][%d-%d-%d-%d:%d:%d]:", Level[level].c_str(), ptm->tm_year + 1900,
                 ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

        // 自定义内容输入
        va_list s;
        va_start(s, format);

        // 我们这里就不对这个format做字符串解析了，我们直接调用对应的函数帮我们做这一步工作
        vsnprintf(custombuffer, sizeof(custombuffer), format, s);

        char logtxt[SIZE * 2];
        snprintf(logtxt, sizeof(logtxt), "%s %s", defaultbuffer, custombuffer);

        PrintLog(level, logtxt);
    }
};


// int sum(int n,...){
//    va_list s;
//    va_start(s,n);//s指向对应的可变参数列表起始地址

//    int sum = 0;
//    while(n--){
//       sum += va_arg(s,int);
//    }

//    return sum;
// }