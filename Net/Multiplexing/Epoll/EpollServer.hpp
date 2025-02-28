#pragma once
#include <memory>
#include "Nocopy.hpp"
#include "Epoller.hpp"
#include "Socket.hpp"

const int defaultport = 8888;

//这里是为Reactor打基础 这里也是只处理EPOLLIN事件
//然后这里的成员都采用智能指针的形式，这样不用做内存管理了，引入shared_ptr所以这里就把
//该类的拷贝构造和赋值重载delete了
//同时我们对Epoll实例进行封装
class EpollServer:public Nocopy{
	static const int num = 32;//这个作为我们能接收的就绪队列的最大个数 anynum
private:
	uint16_t port_;
	shared_ptr<Socket> listenptr_;//指向监听套接字
	shared_ptr<Epoller> epollptr_;//指向epoll实例
public:
	EpollServer(uint16_t port = defaultport):port_(port),listenptr_(new Socket),epollptr_(new Epoller){

	}

	void Init(){
		listenptr_->createSock();
		listenptr_->Bind(port_);
		listenptr_->Listen();

		lg(Info,"EpollServer Init Done...Listensock is:%d",listenptr_->getSockfd());
	}

	void Run(){
		epollptr_->EpollUpdate(EPOLL_CTL_ADD,listenptr_->getSockfd(),EPOLLIN);
		struct epoll_event events[num];
		for(;;){
			int n = epollptr_->EpollWait(events,num);
			if(n > 0){
				lg(Info,"An event ready!");
				Dispatcher(events,n);
			}
			else if(n == 0){
				lg(Info,"Time out...");
			}
			else{
				lg(Error,"Epoll Wait Error...");
				//break;
			}
		}
	}

	void Dispatcher(struct epoll_event queue[],int num){//从就绪队列拿过来的已经就绪的事件
		//这个不用像之前一样遍历自己维护的数组了，这里这两个参数是epollwait的输出型参数 是直接可以遍历的
		for(int i = 0; i < num; i++){	
			if(queue[i].events & EPOLLIN){
				if(queue[i].data.fd == listenptr_->getSockfd()){
					Accepter();
				}
				else{
					Recver(queue[i].data.fd);
				}
			}
			else if(queue[i].events & EPOLLOUT){//写事件就绪

			}
			else{//异常事件就绪

			}
			

		}
	}

	void Accepter(){
		string ip;
		uint16_t port;
		int fd = listenptr_->Accept(&ip,&port);
		if(fd < 0) return;

		//这里就是拿到了对应的新的fd 
		lg(Info, "accept success, %s: %d, sock fd: %d", ip.c_str(), port, fd);
		
		epollptr_->EpollUpdate(EPOLL_CTL_ADD,fd,EPOLLIN);

	}

	void Recver(int fd){//负责读取 这个就是就绪了，可以直接读
		char buffer[512];
		int n = read(fd,buffer,sizeof(buffer) - 1);//这里只是读了 这里的数据要进行处理 我们不确定是否读到一个完整的报文
		if(n > 0){
			buffer[n] = 0;
			cout << "Client say:" << buffer << endl;

			//写回去

		}
		else if(n == 0){
			cout << "client close,me too..." << endl;
			
			//注意这里要先ctl再close，因为epoll_ctl要求对应的fd是有效的
			epollptr_->EpollUpdate(EPOLL_CTL_DEL,fd,0);
			close(fd);
		}
		else{
			lg(Error,"read %d error...",fd);
			close(fd);
			epollptr_->EpollUpdate(EPOLL_CTL_DEL,fd,0);
			return;
		}
	}

	~EpollServer(){
		listenptr_->Close();
	}
};