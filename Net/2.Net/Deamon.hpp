#pragma once
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <string> 
#include <fcntl.h>

void Daemon(const std::string& cwd = ""){
    //1. 忽略相关信号
    signal(SIGCLD,SIG_IGN);
    signal(SIGPIPE,SIG_IGN);
    signal(SIGSTOP,SIG_IGN);//不能被暂停 希望杀死守护进程的唯一方法就是kill -9

    //2. 作为子进程自成一个会话
    if(fork() > 0) exit(0);
    int ret = setsid();
    if(ret < 0){
        std::cerr << "创建会话失败" << std::endl;
        exit(1);
    }

    //3. 更改当前进程的工作目录
    if(!cwd.empty())
       chdir(cwd.c_str());
    
    //4. 重定向标准输入输出错误流到/dev/null
    int fd = open("/dev/null",O_RDONLY);
    if(fd > 0){
        dup2(fd,0);
        dup2(fd,1);
        dup2(fd,2);
        close(fd);
    }
}