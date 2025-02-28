#pragma once
#include <memory>
#include <functional>
#include <unordered_map>
#include "Noblock.hpp"
#include "Nocopy.hpp"
#include "Epoller.hpp"
#include "Socket.hpp"

//在之前的Epoll里，我们没有处理写和异常事件就绪的情况，并且我们读取事件就绪，冲TCP缓冲区读上来的数据只是读了，没有做解析。
//所以我们要给Epoll里的每个fd都维护一个缓冲区，读和写都有，每次读取之后进行保存，这个工作我们做过。
//如果加入了写，写缓冲区如果一直有空余(这很正常)，那写事件就绪就会一直提醒我们，我们不希望这样，所以这里的Epoll是ET模式下的EPoll
//也就是读取数据，写数据，异常处理事件都是循环搞，每个文件都是非阻塞的。——Reactor

class Connection;
class TcpServer;

const int defaultport = 8888;
// using func=function<void(shared_ptr<Connection>)>;
using func=function<void(weak_ptr<Connection>)>;//防止循环引用

//connection每个文件描述符都有对应的connection来管理，都有各自的缓冲区
class Connection{
private:
	int fd_;
	string inbuffer_;
	string outbuffer_;
public:
	func recv_cb_;
	func write_cb_;
	func except_cb_;

	string ip_;
	uint16_t port_;//设置成公开的

	weak_ptr<TcpServer> tcp_ptr_;//回指指针 指向服务器
	//shared_ptr<TcpServer> tcp_ptr_;//回指指针 指向服务器


	Connection(int fd = -1):fd_(fd){

	}

	void SetCallBack(func recv,func send,func except){
		recv_cb_ = recv;
		write_cb_ = send;
		except_cb_ = except;
	}

	void AppendInbuffer(const string& mess){//把接收的数据放到改connection的inbuffer_里
		inbuffer_ += mess;
	}

	void AppendOutBuffer(const string& mess){//同上
		outbuffer_ += mess;
	}

	int getSockfd(){
		return fd_;
	}

	string& getOutBuffer(){//要把这个缓冲区的数据发送出去，这里对外提供一个接口
		return outbuffer_;
	}

	string& getInbuffer(){//外部要对该缓冲区进行读取 然后还要解析 对外提供一个接口
		return inbuffer_;
	}

	void SetWeakPtr(std::weak_ptr<TcpServer> tcp_server_ptr){//?
        tcp_ptr_ = tcp_server_ptr;
    }

	~Connection(){

	}

};

class TcpServer:public std::enable_shared_from_this<TcpServer>, public Nocopy{
	static const int num = 32;
private:
	uint16_t port_;
	shared_ptr<Socket> listenptr_;//监听套接字
	shared_ptr<Epoller> epollptr_;//指向epoll实例
	unordered_map<int,shared_ptr<Connection>> connections_;//fd与对应connection建立的映射关系
	struct epoll_event event_queue[num];//避免传参了 就绪等待队列

	func readfrombuffer_;//tcp的回调函数 绑定应用层的一个函数 把connection缓冲区里的数据读出并作解析
public:
	TcpServer(){
		port_ = -1;
	}

	TcpServer(uint16_t port,func F):port_(port),listenptr_(new Socket),epollptr_(new Epoller){
		readfrombuffer_ = F;
	}

	void Init(){
		listenptr_->createSock();
		//设置成非阻塞
		SetNonBlock(listenptr_->getSockfd());
		listenptr_->Bind(port_);
		listenptr_->Listen();

		lg(Info,"EpollServer Init Done...Listensock is:%d",listenptr_->getSockfd());
		//把listen套接字建立connection添加到connections_里，再添加到内核里去
		//并给listen套接字对应的connection设置对应的回调函数 这个工作都交给AddConnection
		AddConnection(listenptr_->getSockfd(),EPOLLIN|EPOLLET,std::bind(&TcpServer::Accepter,this,
		std::placeholders::_1),nullptr,nullptr);
	}

	void AddConnection(int fd,uint32_t event,func recv,func send,func except,
		const string& ip = "0.0.0.0",uint16_t port = 0){
		//创建connection
		//shared_ptr<Connection> newconnection = make_shared<Connection>(fd);
		shared_ptr<Connection> newconnection(new Connection(fd));
		newconnection->SetCallBack(recv,send,except);
		newconnection->ip_ = ip;
		newconnection->port_ = port;

		newconnection->SetWeakPtr(shared_from_this()); // shared_from_this(): 返回当前对象的shared_ptr ?

		//添加到内核中
		epollptr_->EpollUpdate(EPOLL_CTL_ADD,fd,event);
		//构建映射关系
		connections_[fd] = newconnection;
	}

	void Loop(){
		//epollptr_->EpollUpdate(EPOLL_CTL_ADD,listenptr_->getSockfd(),EPOLLIN|EPOLLET);
		//把listen套接字添加到内核 并且设置ET模式
			
		while(true){
			int n = epollptr_->EpollWait(event_queue,num);
			if(n > 0){//有数据就绪
				Dispatcher(n);
			}
			else if(n == 0){
				lg(Info,"Time out...");
			}
			else{
				lg(Warning,"Wait Error...");
			}
		}
	}

	bool IsConnectionSafe(int fd){
		auto it = connections_.find(fd);
		return it != connections_.end() ? true : false;
	}

	void Dispatcher(int n){
		for(int i = 0; i < n; i++){
			int fd = event_queue[i].data.fd;
			int event = event_queue[i].events;

			//把异常的等待情况 都处理成读写异常
			// if(event & EPOLLHUP){
			// 	event |= (EPOLLIN | EPOLLOUT);
			// }
			// if(event & EPOLLERR){
			// 	event |= (EPOLLIN | EPOLLOUT);
			// }

			if((event & EPOLLIN) && IsConnectionSafe(fd)){
				if(connections_[fd]->recv_cb_)
					connections_[fd]->recv_cb_(connections_[fd]);
			}
			else if((event & EPOLLOUT) && IsConnectionSafe(fd)){
				if(connections_[fd]->write_cb_)
					connections_[fd]->write_cb_(connections_[fd]);
			}
		}
	}

	//连接管理器 作为listen套接字的recv回调函数 等到listen套接字事件就绪 就回调这个
	void Accepter(weak_ptr<Connection> conn){
		if(conn.expired()) return;
		auto con = conn.lock();

		//别忘了我们是ET模式，读就要读干净
		while(true){
			string ip;
			uint16_t port;
			int fd = listenptr_->Accept(&ip,&port);
			if(fd > 0){
				//别忘了我们是ET模式
				SetNonBlock(fd);	
				lg(Debug, "get a new client, get info-> [%s:%d], sockfd : %d", ip.c_str(), port, fd);

				//接收了一个套接字就把他创建connection，加入内核，假如映射表里
				//同时设置普通fd的三个回调函数(listen的只需要设置recvcb就行了)
				AddConnection(fd,EPOLLIN|EPOLLET,std::bind(&TcpServer::Recver,this,std::placeholders::_1),
				            std::bind(&TcpServer::Writer,this,std::placeholders::_1),
				            std::bind(&TcpServer::Excepter,this,std::placeholders::_1),
						    ip,port);
			}
			else{
				if(errno == EWOULDBLOCK) break;//非阻塞的返回
				else if(errno == EINTR) continue;//信号中断-不管
				else{//错误返回
					break;//这个Accept里有对应的日志，这里就不打印了
				}
			}
		}
		
	}

	//接收器 只负责通过read读到con里的对应的写入缓冲区(本质就是从tcp的接收缓冲区拷贝到我们的)
	void Recver(weak_ptr<Connection> conn){
		if(conn.expired()) return;
		auto con = conn.lock();


		int fd = con->getSockfd();

		while(true){
			char buffer[1024];
			//int n = read(fd,buffer,sizeof(buffer) - 1);
			int n = recv(fd,buffer,sizeof(buffer) - 1,0);//这里要非阻塞读取(最后一个参数设为零)
			if(n > 0){
				buffer[n] = 0;
				//cout << "Client say:" << buffer << endl;
				con->AppendInbuffer(buffer);
			}
			else if(n == 0){
				lg(Info,"Client close its fd: %d....",fd);
				con->except_cb_(con);
				return;
			}
			else{
				if(errno == EWOULDBLOCK) break;//没有事件就绪
				else if(errno == EINTR) continue;
				else{
					lg(Error,"read sockfd %d error...",fd);
					con->except_cb_(con);
					return;
				}
			}
		}

		//搞完之后 写回应用层
		readfrombuffer_(con);
	}

	//我们对读取是常关心，对写入则是按需设置，因为写这个事件如果是常关心，那他就会一直就绪(大部分时间)
	//这里我们的处理是 第一次读直接读完，读完之后如果outbuffer里还有数据，再设置写关心
	//为什么再Accepter时对fd设置的event是EPOLLIN，没有EPOLLOUT因为我们是服务器，不会主动发送消息的
	//只有收到相关请求才会进行write
	void Writer(weak_ptr<Connection> conn){
		if(conn.expired()) return;
		auto con = conn.lock();

		int fd = con->getSockfd();
		string outbuffer = con->getOutBuffer();
		while(true){
			int n = send(fd,outbuffer.c_str(),outbuffer.size(),0);//非阻塞
			if(n > 0){
				outbuffer.erase(0,n);
				if(outbuffer.empty()) break;
			}
			else if(n == 0){//对方关闭了连接 同时也没数据发送
				return ;
			}
			else{//n < 0写出错
				if(errno = EWOULDBLOCK) break;
				if(errno = EINTR) continue;
				else{//真出错了
					lg(Error,"Write fd:%d error...",fd);
					con->except_cb_(con);
					return;
				}
			}
		}

		if(!outbuffer.empty()){//没空 就写关心
			ModEpollEvent(fd,true,true);
		}
		else{//空了 就不关心了
			ModEpollEvent(fd,true,false);
		}

	}

	void ModEpollEvent(int fd,bool readable,bool writable){
		uint32_t event = 0;
		event |= ((readable ? EPOLLIN  : 0) | (writable ? EPOLLOUT : 0) | EPOLLET);
		epollptr_->EpollUpdate(EPOLL_CTL_MOD,fd,event);
	}

	//读写异常都会到这里
	void Excepter(weak_ptr<Connection> conn){
		if(conn.expired()) return;		
		auto con = conn.lock();

		int fd = con->getSockfd();

		//1 从内核中移除
		epollptr_->EpollUpdate(EPOLL_CTL_DEL,fd,0);

		//2 close
		lg(Info,"Close the except fd: %d",fd);
		close(fd);

		//3 从connections里移走
		lg(Info,"Remove fd:%d from connections",fd);
		connections_.erase(fd);
	}

	~TcpServer(){
		listenptr_->Close();
	}
};
