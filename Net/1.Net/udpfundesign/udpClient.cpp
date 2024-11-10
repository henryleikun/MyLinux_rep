#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <string>
#include <pthread.h>
#include "log.hpp"
using namespace std;

Log lg;

void Usage(const string& proc){//用法
    cout << "\n\rUsage: " << proc << " port[1024+]  IP\n" << endl;
}

struct ThreadData{
   int sockfd;
   struct sockaddr_in server;
   string serverip;//减少转化次数
};

//线程来执行向服务端发送信息
void* Send(void* args){
   ThreadData* td = static_cast<ThreadData*>(args);

   //每个客户启动的时候，会先发一个谁谁来了的信息
   string welcome = td->serverip;
   welcome += "coming.......";
   sendto(td->sockfd,welcome.c_str(),welcome.size(),0,(sockaddr*)&(td->server),sizeof(td->server));
   
   string outbuffer;
   while(1){
        cout << "Please enter@ ";
        getline(cin,outbuffer);

        sendto(td->sockfd,outbuffer.c_str(),outbuffer.size(),0,(sockaddr*)&(td->server),sizeof(td->server));
   }
}

//线程执行接收服务端发来的信息
void* Recv(void* args){
   ThreadData* td = static_cast<ThreadData*>(args);
   
   char inbuffer[1024];//用来接收服务端的响应信息
   while(1){
      struct sockaddr_in recv;
      socklen_t length = sizeof(recv);
    
      int recvn = recvfrom(td->sockfd,inbuffer,sizeof(inbuffer) - 1,0,(sockaddr*)&recv,&length);
      if(recvn > 0){
          inbuffer[recvn] = 0;
          cerr << inbuffer << endl;
      }
   }
}

int main(int argc,char* argv[]){
    if(argc != 3){
        Usage(argv[0]);
        exit(0);
    }

    //创建套接字
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0) {
        lg(Fatal,"socket create error,sockfd:%d",sockfd);
        exit(1);
    }

    //下面只需要知道，要发给谁就行了
    ThreadData td;
    u_int16_t port = stoi(argv[1]);
    string ip = argv[2];
    bzero(&td.server,sizeof(td.server));
    
    td.sockfd = sockfd;
    td.server.sin_family = AF_INET;
    td.server.sin_port = htons(port);
    td.server.sin_addr.s_addr = inet_addr(ip.c_str());
    td.serverip = ip;
    
    pthread_t sender,recver;
    //考虑到每个线程执行的函数需要清楚发给谁(server的addr_in信息和套接字信息),所以这里传参要传他俩——搞个结构体打包起来
    pthread_create(&sender,nullptr,Send,&td);
    pthread_create(&recver,nullptr,Recv,&td);

    pthread_join(sender,nullptr);
    pthread_join(recver,nullptr);
    
    close(sockfd);
    return 0;
}

