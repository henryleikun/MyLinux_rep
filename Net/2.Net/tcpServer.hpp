#pragma once
#include <iostream>
#include <string>
#include <cstdint>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>
#include "log.hpp"
#include "ThreadPool.hpp"
#include "Deamon.hpp"
using namespace std;

Log lg;

enum{
    SOCKET_ERROR = 1,
    BIND_ERROR,
    LISTEN_ERROR
};

string defaultip = "0.0.0.0";
uint16_t defaultport = 8888;
int backlog = 10;

class TcpServer;

struct ThreadData{
   string ip;
   uint16_t port;
   int sockfd;
   TcpServer* p_sv;
};

class TcpServer{
private:
   int listensock_;
   string ip_;
   uint16_t port_;
public:
   TcpServer(const uint16_t& port = defaultport,const string& ip = defaultip)
   :ip_(ip),port_(port),listensock_(-1){

   }

   void Init(){//初始化服务器 为什么不放在构造函数里——构造函数一般不放一些有风险的事
      listensock_ = socket(AF_INET,SOCK_STREAM,0);
      if(listensock_ < 0){
        lg(Fatal,"Socket create error,listensock_:%d,error str:%s",listensock_,strerror(errno));
        exit(SOCKET_ERROR);
      }
      lg(Info,"Socket create success,listensock_:%d,error str:%s",listensock_,strerror(errno));
      
      // 防止偶发性的服务器无法进行立即重启
      int opt = 1;
      setsockopt(listensock_, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt)); 

      //进行套接字绑定
      struct sockaddr_in local;
      memset(&local,0,sizeof(local));
      local.sin_family = AF_INET;
      local.sin_port = htons(port_);
      inet_aton(ip_.c_str(),&(local.sin_addr));//换个用

      if(bind(listensock_,(sockaddr*)&local,sizeof(local)) < 0){
        lg(Fatal,"bind error,errno:%d,error str:%s",errno,strerror(errno));
        exit(BIND_ERROR);
      }
      lg(Info,"bind success,errno:%d,error str:%s",errno,strerror(errno));

      int ret = listen(listensock_,backlog);
      if(ret < 0){
        lg(Fatal,"listen error,errno:%d,error str:%s",errno,strerror(errno));
        exit(LISTEN_ERROR);
      }
      lg(Info,"listen success,listensock:%d",listensock_);
   }

   static void* ThreadRun(void* args){
       pthread_detach(pthread_self());
       ThreadData* td = static_cast<ThreadData*>(args);
       
       td->p_sv->Service(td->sockfd,td->ip,td->port);
       delete td;
       return nullptr;
   }

   void Run(){
      Daemon();
      //Deamon("/");//注意使用根目录时的文件路径相关问题。
      //signal(SIGPIPE,SIG_IGN);
      //服务端的启动带动线程池的启动
      ThreadPool<Task>::GetInstance()->Start();//10个线程的线程池
      lg(Info,"TcpServer is runinng...");
      while(1){
          //获取客户端的连接
          struct sockaddr_in client;
          socklen_t len = sizeof(client);
          int sockfd = accept(listensock_,(sockaddr*)&client,&len);//没有连接会在这里阻塞住
          if(sockfd < 0){//建立连接失败 但这不致命 继续建立下一个连接就行
             lg(Warning,"accept error,errno:%d,errstr:%s",errno,strerror(errno));
             continue;
          }
          
          char clientip[16];
          inet_ntop(AF_INET,&(client.sin_addr),clientip,sizeof(clientip));
          uint16_t clientport = ntohs(client.sin_port);
          //lg(Info,"Get a new link,%s:%d",clientip,clientport);
      
          //连接建立成功 申请连接的客户端的信息 都在client里保存 并且也知道对应的通信套接字的文件描述符 
          //下面可以根据上述这些进行通信 上面的信息都是通信的必要信息，作为参数放到一个函数里去执行相关通信
          
          //version 1 单进程单线程
          //Service(sockfd,clientip,clientport);

          //version 2 多进程
          // int id = fork();//这里就不判断创建进程失败了
          // if(id == 0){//子进程
          //   close(listensock_);
          //   if(fork() > 0) exit(0);
          //   Service(sockfd,clientip,clientport);
          //   exit(0);//子进程执行完直接退出——让系统回收
          // }
          
          // close(sockfd);
          // //父进程
          // int rid = waitpid(id,nullptr,0);//这里就只是回收，不关心其情况

          //version 3 多线程
          // ThreadData* td = new ThreadData;//这里注意不要用局部变量，要不然会线程执行函数里野指针
          // td->ip = clientip;
          // td->port = clientport;
          // td->p_sv = this;
          // td->sockfd = sockfd;

          // pthread_t tid;
          // pthread_create(&tid,nullptr,ThreadRun,td);

          //version 4 线程池
          Task t(sockfd,clientip,clientport);//我希望我通过对应sockfd和对应ip,port能构建一个服务的任务
          ThreadPool<Task>::GetInstance()->Push(t);
      } 

   }

   void Service(int sockfd,const string& ip,const uint16_t& port){
      char inbuffer[4096];
      while(1){//先搞个长服务(目前是单进程单线程),服务一个客户服务完之后才能进行下次连接 并且这个服务是一直服务反正是测试
        //cout << "before reading..." << endl;
        int n = read(sockfd,inbuffer,sizeof(inbuffer));
        //cout << "after reading..." << endl;
        if(n > 0){//读到了
          inbuffer[n] = 0;
          cout << "client say# " << inbuffer << endl;
          string echo_string = "Sever echo#";
          echo_string += inbuffer;
          
          int wn = write(sockfd,echo_string.c_str(),echo_string.size());
          if(wn < 0){//向客户写入失败
            lg(Fatal,"Write error,server will close the connection");
            break;
          }
        }
        else if(n == 0){//客户端挂掉(客户端主动or被动关闭了sockfd)
          lg(Info,"%s:%d close its sockfd,Server will recv next client...",ip.c_str(),port);
          break;
        }
        else{//n < 0读出错
          lg(Warning,"Server read the client wrongly,closing the server sockfd...");
          break;
        }

      }
      
      close(sockfd);//服务完之后 不忘了关闭对应的sockfd(为客户端提供服务的sockfd)
   }

   ~TcpServer(){
     close(listensock_);
   }
};