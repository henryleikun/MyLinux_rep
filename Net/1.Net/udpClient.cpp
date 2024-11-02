#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <string>
#include "log.hpp"
using namespace std;

Log lg;

void Usage(const string& proc){//用法
    cout << "\n\rUsage: " << proc << " port[1024+]  IP\n" << endl;
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

    //客户端不用bind，底层会自动bind，端口号也是OS给你分配的唯一的
    //其实client的port是多少，其实不重要，只要能保证主机上的唯一性就可以
    //系统什么时候给我bind呢？首次发送数据的时候

    //下面只需要知道，要发给谁就行了
    u_int16_t port = stoi(argv[1]);
    string ip = argv[2];
    struct sockaddr_in server;
    bzero(&server,sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip.c_str());

    string outbuffer;
    char inbuffer[1024];//用来接收服务端的响应信息
    while(1){ 
        cout << "Please enter@ ";
        getline(cin,outbuffer);
        //cin >> outbuffer;
        
        //client先发送
        sendto(sockfd,outbuffer.c_str(),outbuffer.size(),0,(sockaddr*)&server,sizeof(server));
        
        //然后接收服务器发来的信息
        struct sockaddr_in recv;
        socklen_t length = sizeof(recv);
        
        //cout << "before recving...." << endl;
        int recvn = recvfrom(sockfd,inbuffer,sizeof(inbuffer) - 1,0,(sockaddr*)&recv,&length);
        //cout << "after recving...." << endl;
        if(recvn > 0){
            inbuffer[recvn] = 0;
            cout << inbuffer << endl;
        }
    }
    
    close(sockfd);
    return 0;
}

