#pragma once
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>

//Epoll实例主要是主要就是涉及到，epoll创建，epoll控制(增删改)，epoll等待(等待就绪队列)
class Epoller{
private:
	int epollfd_;
	int timeout = 3000;//默认3s
public:
	Epoller(){
		epollfd_ = epoll_create(1);//参数大于零就行 这个参数已经废弃了
		if(epollfd_ < 0){
			std::cerr << "Epoll create error..." << std::endl;
			exit(1);
		}

		std::cout << "Create epoll success! fd:" << epollfd_ << std::endl;
	}

	int EpollUpdate(int op,int fd,uint32_t event_care){//op决定做哪种操作 fd是对谁做，event是当增或改的时候想关心的事件
		int ret = 0;
		if(op == EPOLL_CTL_DEL){
		    ret = epoll_ctl(epollfd_,op,fd,nullptr);
			if(ret < 0){
				std::cout << "EpollCtl error,Error fd: " << fd;
				cout << "  " << strerror(errno) << endl;
			}
		}
		else{
			//EPOLL_CTL_ADD/EPOLL_CTL_MOD
			struct epoll_event event;
			event.events = event_care;
			event.data.fd = fd;
			ret = epoll_ctl(epollfd_,op,fd,&event);//本质就是在epoll实例的红黑树中更新 提供fd的要关心的事件
			if(ret < 0){
				std::cout << "EpollCtl error,Error fd: " << fd;
				cout << "  " << strerror(errno) << endl;
			}
		}

		return ret;
	}

	int EpollWait(struct epoll_event epoll_queue[],int maxevents){//返回等待了多少个fd
		int ret = epoll_wait(epollfd_,epoll_queue,maxevents,timeout);
		return ret;
	}


	~Epoller(){
		if(epollfd_ >= 0)
			close(epollfd_);
	}
};