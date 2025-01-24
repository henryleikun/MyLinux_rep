#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>//sockadd_in
#include <arpa/inet.h>//ip的inaddr和char*的转换
#include <cstring>
#include "log.hpp"

Log lg;
int backlog = 10;

enum{
    SockErr = 1,
    BindErr,
    ListenErr,
    ConErr
};

class Socket{
private:
    int sockfd_;
public:
    Socket(){}
    ~Socket(){}

    void createSock(){
        sockfd_ = socket(AF_INET,SOCK_STREAM,0);
        if(sockfd_ < 0){
            lg(Fatal,"Socket error,%s,%d",strerror(errno),errno);
            exit(SockErr);
        }
    }

    void Bind(uint16_t port){
        struct sockaddr_in local;
        memset(&local,0,sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;

        if(bind(sockfd_,(sockaddr*)&local,sizeof(local)) < 0){
            lg(Fatal,"Bind error,%s,%d",strerror(errno),errno);
            exit(BindErr);
        }        
    }

    void Listen(){
        int ret = listen(sockfd_,backlog);
        if(ret < 0){
            lg(Fatal,"Listen error,%s,%d",strerror(errno),errno);
            exit(ListenErr);
        }
    }

    //需要两个输出型参数，服务器需要知道是谁发来的连接
    int Accept(string* ip,uint16_t* port){
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int sockfd = accept(sockfd_,(sockaddr*)&client,&len);
        if(sockfd < 0){
            lg(Warning,"Accept error,%s,%d",strerror(errno),errno);
            return -1;
        }
        *port = ntohs(client.sin_port);
        //ip(int)转string
        char tempip[16];
        inet_ntop(AF_INET,&client.sin_addr,tempip,sizeof(tempip));
        *ip = tempip;

        return sockfd;
    }

    void Connect(const string& ip,uint16_t& port){//客户端负责 需要知道向谁发起连接
        //客户端的bind是在connect的逻辑里的
        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        inet_aton(ip.c_str(),&server.sin_addr);//string转in_addr
        
        int ret = connect(sockfd_,(sockaddr*)&server,sizeof(server));
        if(ret < 0){
            lg(Error,"Connect error,%s,%d",strerror(errno),errno);
            exit(ConErr);
        }
    }

    void Close(){
        close(sockfd_);
    }

    int getSockfd()const{
        return sockfd_;     
    }
};