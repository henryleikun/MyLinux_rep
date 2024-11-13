#pragma once
#include <iostream>
#include <string>
#include "log.hpp"
#include "Dictionary.hpp"

extern Log lg;

class Task{
private:
   int sockfd_;
   string ip_;
   uint16_t port_;
public:
   Task() = default;

   Task(int sockfd,const string& ip,const uint16_t& port)
   :sockfd_(sockfd),ip_(ip),port_(port){
   
   }

   void Run(){//执行Service的逻辑
      Dictionary dict;
      dict.Init();
      char inbuffer[4096];
      int n = read(sockfd_,inbuffer,sizeof(inbuffer));
      if(n > 0){//读到了
         inbuffer[n] = 0;
         lg(Info,"Client sends the words:%s",inbuffer);
         string echo_string = dict.Find(inbuffer);
         //cout << "Find result:" << echo_string << endl;
         
         // close(sockfd_);
         // sleep(5);//这个期间让客户端挂掉
         
         int wn = write(sockfd_,echo_string.c_str(),echo_string.size());
         if(wn < 0){//向客户写入失败
           lg(Fatal,"Write error,server will close the connection");
         }
      }    
      else if(n == 0) //客户端挂掉(客户端主动or被动关闭了sockfd)
         lg(Info,"%s:%d close its sockfd,Server will recv next client...",ip_.c_str(),port_);
      else //n < 0读出错
         lg(Warning,"Server read the client wrongly,closing the server sockfd...");
      
      close(sockfd_);//服务完之后 不忘了关闭对应的sockfd(为客户端提供服务的sockfd)
   }

   void operator()(){
      Run();
   }

   ~Task(){
   }
};
