#pragma once
#include <iostream>
#include <sys/select.h>
#include "Socket.hpp"

using namespace std;
const int defaultport = 8888;
const int fd_num_max = sizeof(fd_set) * 8;
const int defaultfd = -1;

//目前只处理读事件
class SelectServer{
private:
	Socket listensock_;
	uint16_t port_;
	int fds_[fd_num_max];
public:
	SelectServer(uint16_t port = defaultport):port_(port){
		for(int i = 0; i < fd_num_max; i++)
			fds_[i] = defaultfd;
	}

	void Init(){
		listensock_.createSock();
		listensock_.Bind(port_);
		listensock_.Listen();
		lg(Info,"Select Server Init done...");
	}

	void Dispatcher(fd_set resp_fds){//对就绪的事件做分发处理
		//这里我们处理的文件fd一个是listen的，一个是普通读的所以我们要进行区分 并且编写不同的处理方法
		for(int i = 0; i < fd_num_max; i++){//一次遍历
			if(FD_ISSET(fds_[i],&resp_fds)){
				if(fds_[i] == listensock_.getSockfd()){
					Accepter();
				}
				else{
					Recver(fds_[i],i);
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
		//要把它设置进我们的fds_，以便下次select能知道他的情况 所以我们要在fds_里找一个合法的位置
		
		int i = 1;
		for(; i < fd_num_max; i++){//两次遍历
			if(fds_[i] == defaultfd){
				fds_[i] = fd;
				break;
			}
		}

		if(i == fd_num_max){//到fd_set(select)的上限了
			lg(Warning,"Fd is too much,Please connect later...");
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
			fds_[pos] = defaultfd;
		}
		else{
			lg(Error,"read %d error...",fd);
			close(fd);
			fds_[pos] = defaultfd;
			return;
		}
	}

	void Run(){
		fds_[0] = listensock_.getSockfd();	

		
		for(;;){
			//考虑到select参数的特殊性 每次进行select都要重新对fds进行设置
			fd_set set_fds;	
			FD_ZERO(&set_fds);

			int maxfd = fds_[0];
			FD_SET(fds_[0],&set_fds);
			for(int i = 1; i < fd_num_max; i++){//三次遍历
				if(fds_[i] == defaultfd)
					continue;
				
				FD_SET(fds_[i],&set_fds);
				if(fds_[i] > maxfd) maxfd = fds_[i];
			}

			struct timeval gap = {3 , 0};
			//gap.tv_sec = 1;//1s一次
			int n = select(maxfd+1,&set_fds,nullptr,nullptr,&gap);//这一步之后set_fds已经变了
			if(n > 0){//有事件就绪
				lg(Info,"An event is ready");
				Dispatcher(set_fds);
			}
			else if(n == 0){//超时
				lg(Info,"Time out.Step in next circular");
			} 
			else{//出错
				// switch (errno) {
				// 	case EBADF:
				// 		fprintf(stderr, "Invalid file descriptor in select.\n");
				// 		break;
				// 	case EINTR:
				// 		// 信号中断，可再次调用 select
				// 		break;
				// 	case EINVAL:
				// 		fprintf(stderr, "Invalid argument passed to select.\n");
				// 		break;
				// 	case ENOMEM:
				// 		fprintf(stderr, "Out of memory for select operation.\n");
				// 		break;
				// 	default:
				// 		fprintf(stderr, "Unknown error in select.\n");
				// 		break;
				// }
				lg(Error,"Select Error");
				//break;
			}
		}
	}

	~SelectServer(){
		listensock_.Close();
	}

};