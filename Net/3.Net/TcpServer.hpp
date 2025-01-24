#pragma once
#include <signal.h>
#include <functional>
#include "Socket.hpp"

using func = std::function<string(string&)>;

class TcpServer{
private:
    uint16_t port_;//服务器的端口
    Socket listensock_;//接听套接字
    func callback_;
public:
    TcpServer(uint16_t port,func callback):port_(port),callback_(callback){
        
    }

    void InitServer(){
        listensock_.createSock();
        listensock_.Bind(port_);
        listensock_.Listen();
        lg(Info,"TcpServer init...Done");
    }

    void Run(){
        signal(SIGCHLD,SIG_IGN);
        signal(SIGPIPE,SIG_IGN);

        while(true){
            string ip;
            uint16_t port;
            int sockfd = listensock_.Accept(&ip,&port);
            if(sockfd < 0) continue;//这个没连上
            lg(Info,"client:[%s:%d]connected...",ip.c_str(),port);

            //收到一个客户端的连接后进行处理——>这里不是重点，可以用线程池去搞但不想让代码冗长
            //这里就直接用多进程搞定了
            if(fork() == 0){//子进程
                listensock_.Close();//子进程不用监听 关掉(有计数器)
                
                //服务逻辑
                string inbuffer_stream;//用来保存该客户端发来的所有计算请求 类似一个请求队列
                while(true){
                    //这个接收是用户缓冲区的接收 这里的内容可以是半个，一个，多个报文请求
                    char buffer[1024];//接收客户端发来的请求
                    int n = read(sockfd,buffer,sizeof(buffer));
                    if(n > 0){
                        buffer[n] = 0;
                        inbuffer_stream += buffer;
                        lg(Debug,"Debug-inbufferstream:\n%s",inbuffer_stream.c_str());

                        while(true){
                            string response = callback_(inbuffer_stream);
                            if(response.size() == 0){//说明报文有问题
                                break;
                            }
                            
                            //下面两个打印是为了测试客户端一次发来多个请求时 我们服务端是能hold的住的
                            //能一次处理多个请求 处理完之后在进行下次的接收
                            lg(Debug,"Response:\n%s",response.c_str());
                            lg(Debug,"inbuffer:\n%s",inbuffer_stream.c_str());

                            write(sockfd,response.c_str(),response.size());
                        }
                    }
                    else{
                        close(sockfd);
                        break;//直接关闭当前进程
                    }
                }

                exit(0);//我不想让父进程等待子进程(可以用子子进程解决)，这里直接把信号忽略掉
            }

            close(sockfd);//父进程关闭对应的sockfd接着去监听下一个
        }

    }

    ~TcpServer(){

    }

};