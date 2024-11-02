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
#include "log.hpp"
using namespace std;

enum{
    SOCKET_ERROR = 1,
    BIND_ERROR
};

Log lg;
typedef function<string(string&)> Func;

string defaultip = "0.0.0.0";
u_int16_t defaultport = 8888;//尽量大点

class UdpServer{//封装一个UDP下的服务器
private:
   string ip_;//绑定的时候得知道本机的ip ip在应用层一般都是字符串的形式
   u_int16_t port_;//2B的端口号
   int sockfd_;//网路文件(网卡文件)的文件描述符
   bool isrunning_;//服务器是否在运行的状态表示
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

   void Run(Func Handle){//服务端一般都是一直在跑的——死循环
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

         //获取到了信息要对信息进行处理了
         inbuffer[recvn] = 0;//当C字符串来处理
         string temp = inbuffer;
         cout << inbuffer << endl;//先打印在服务器上
         string ret = Handle(temp);//搞个包装器来做函数对象 这个函数对象可以作为Run的参数
                                //这里可以直接传inbuffer 但包装器的参数要是右值引用string&&
    
         //处理完了 发给client
         sendto(sockfd_,ret.c_str(),ret.size(),0,(sockaddr*)&client,length);
      }
   }

   ~UdpServer(){
     if(sockfd_ > 0) 
        close(sockfd_);
   }
};
