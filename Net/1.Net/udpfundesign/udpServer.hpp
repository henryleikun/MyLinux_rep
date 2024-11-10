#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <cstring>
#include <cstdlib>
#include <functional>
#include <unordered_map>
#include "log.hpp"
using namespace std;

enum{
    SOCKET_ERROR = 1,
    BIND_ERROR
};

Log lg;
//typedef function<string(const string&)> Func;
typedef function<string(const string&,uint16_t,const string&)> Func;

string defaultip = "0.0.0.0";
u_int16_t defaultport = 8888;//尽量大点

class UdpServer{//封装一个UDP下的服务器
private:
   string ip_;//绑定的时候得知道本机的ip ip在应用层一般都是字符串的形式
   u_int16_t port_;//2B的端口号
   int sockfd_;//网路文件(网卡文件)的文件描述符
   bool isrunning_;//服务器是否在运行的状态表示

   unordered_map<string,sockaddr_in> online;//在线的客户(ip-套接字)
public:
   UdpServer(u_int16_t port = defaultport,const string& ip = defaultip)
   :ip_(ip),port_(port),isrunning_(false),sockfd_(0){
    
   }

   void Init(){//初始化服务器 做的工作是 创建套接字 绑定相关信息
      //创建套接字
      sockfd_ = socket(AF_INET,SOCK_DGRAM,0);//注意是UDP
      if(sockfd_ < 0) {
        lg(Fatal,"socket create error,sockfd:%d",sockfd_);
        exit(SOCKET_ERROR);
      }
      lg(Info,"socket create success,sockfd:%d",sockfd_);

      //进行bind 绑定服务器的IP和port
      struct sockaddr_in local;
      bzero(&local,sizeof(local));
      local.sin_family = AF_INET;
      local.sin_port = htons(port_);
      //local.sin_addr.s_addr = stoi(ip_);
      local.sin_addr.s_addr = inet_addr(ip_.c_str());

      if(bind(sockfd_,(sockaddr*)&local,sizeof(local)) < 0){
         lg(Fatal,"bind error, errno: %d, err string: %s",errno,strerror(errno));
         exit(BIND_ERROR);
      }

      lg(Info,"bind success, errno: %d, err string: %s",errno,strerror(errno));
   }

   void CheckUser(sockaddr_in& client,const string& ip,uint16_t port){
      if(!online.count(ip)){//没在在线表里
         online[ip] = client;
         cout << "[" << ip << ":" << port << "] add to online user." << endl; 
      }
   }

   void Broadcast(const string& mes,const string& ip,uint16_t port){
      for(auto& e : online){
         string message = "[";
         message += ip;
         message += ":";
         message += std::to_string(port);
         message += "]# ";
         message += mes;
         socklen_t length = sizeof(e.second);
         sendto(sockfd_,message.c_str(),message.size(),0,(sockaddr*)&(e.second),length);
      }
   }

   void Run(/*Func Handle*/){//服务端一般都是一直在跑的——死循环
      //对于服务端，肯定是先要接收client的消息。去做回应
      isrunning_ = true;
      cout << "is running......" << endl;
      char inbuffer[SIZE];//接收client发来的信息
      while(isrunning_){
         struct sockaddr_in client;//保存client的ip和端口信息(网络协议族信息) 知道是谁发过来的
         socklen_t length = sizeof(client);
         //cout << "before recving...." << endl;
         int recvn = recvfrom(sockfd_,inbuffer,sizeof(inbuffer) - 1,0,(sockaddr*)&client,&length);
         //cout << "after recving....." << endl;
         if(recvn < 0){
            lg(Warning,"recvfrom error, errno: %d, err string: %s",errno, strerror(errno));
            continue;
         }
         
         //拿到对应发来的客户的ip和port
         uint16_t client_port = ntohs(client.sin_port);
         char tempip[16];//最大"255.255.255.255"
         string client_ip = inet_ntop(AF_INET,&(client.sin_addr),tempip,sizeof(tempip));
         CheckUser(client,client_ip,client_port);

         //获取到了信息要对信息进行处理了
         inbuffer[recvn] = 0;//当C字符串来处理
         string temp = inbuffer;
         // string ret = Handle(temp);
         //string ret = Handle(temp,client_port,client_ip);
    
         //处理完了 发给client
         //sendto(sockfd_,ret.c_str(),ret.size(),0,(sockaddr*)&client,length);
         //现在，我要把inbuffer的信息广播给所有在线客户
         Broadcast(temp,client_ip,client_port);
      }
   }

   ~UdpServer(){
     if(sockfd_ > 0) 
        close(sockfd_);
   }
};
