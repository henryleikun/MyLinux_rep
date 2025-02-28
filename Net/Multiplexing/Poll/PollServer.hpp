#pragma once
#include <iostream>
#include <poll.h>
#include "Socket.hpp"

using namespace std;
const int defaultport = 8888;
const int fd_num_max = 128;
const int defaultfd = -1;
const int non_event = 0;

//目前只处理读事件
//这里poll跟select不同 用一个pollfd数组管理所有的fds，然后这是个结构体，封装的是对应的要关心的事件和事件就绪情况
class PollServer{
private:
	Socket listensock_;
	uint16_t port_;
	struct pollfd fds_[fd_num_max];//这个是没有上限的
public:
	PollServer(uint16_t port = defaultport):port_(port){
		for(int i = 0; i < fd_num_max; i++){
			fds_[i].fd = defaultfd;
			fds_[i].events = non_event;
			fds_[i].revents = non_event;
		}		
	}

	void Init(){
		listensock_.createSock();
		listensock_.Bind(port_);
		listensock_.Listen();
		lg(Info,"Select Server Init done...");
	}

	void Dispatcher(){//对就绪的事件做分发处理
		//直接对fds_进行遍历
		for(int i = 0; i < fd_num_max; i++){//一次遍历
			//if(fds_[i].fd != defaultfd){//不是你要看事件是否就绪，而不是看是不是我们关心的
			if(fds_[i].revents & POLLIN){
				if(fds_[i].fd == listensock_.getSockfd()){
					Accepter();
				}
				else{
					Recver(fds_[i].fd,i);
				}
			}
		}

	}

	void Accepter(){//接收器 listen套接字的处理方法
		//这里accept已经就绪，绝对不会阻塞
		string ip;
		uint16_t port;
		int fd = listensock_.Accept(&ip,&port);
		if(fd < 0) return;

		//这里就是拿到了对应的新的fd 
		lg(Info, "accept success, %s: %d, sock fd: %d", ip.c_str(), port, fd);
		//要把它设置进我们的fds_，以便下次poll等待能知道他的情况 所以我们要在fds_里找一个合法的位置
		
		int i = 1;
		for(; i < fd_num_max; i++){//二次遍历 找位置
			if(fds_[i].fd == defaultfd){
				fds_[i].fd = fd;
				fds_[i].events = POLLIN;
				break;
			}
		}

		if(i == fd_num_max){//扩容，这里就不扩了
			close(fd);
			return;
		}
	}

	void Recver(int fd,int pos){//负责读取 这个就是就绪了，可以直接读
		char buffer[512];
		int n = read(fd,buffer,sizeof(buffer) - 1);//这里只是读了
		if(n > 0){
			buffer[n] = 0;
			cout << "Client say:" << buffer << endl;
		}
		else if(n == 0){
			cout << "client close,me too..." << endl;
			close(fd);
			fds_[pos].fd = defaultfd;
		}
		else{
			lg(Error,"read %d error...",fd);
			close(fd);
			fds_[pos].fd = defaultfd;
			return;
		}
	}
	
	void Run(){
		fds_[0].fd = listensock_.getSockfd();
		fds_[0].events = POLLIN;	
		
		for(;;){
			//Poll
			int n = poll(fds_,fd_num_max,3000);//第三个参数是毫秒
			if(n > 0){//有事件就绪
				lg(Info,"An event is ready");
				Dispatcher();
			}
			else if(n == 0){//超时
				lg(Info,"Time out.Step in next circular");
			} 
			else{//出错
				lg(Error,"Select Error");
				break;
			}
		}
	}

	~PollServer(){
		listensock_.Close();
	}

};
