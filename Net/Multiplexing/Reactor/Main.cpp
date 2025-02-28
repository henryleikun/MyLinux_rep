#include <iostream>
#include <memory>
#include "CalServer.hpp"
#include "TcpSever.hpp"

void Usage(const string& proc){
    cout << "Usage:"<<endl;
    cout << "\t" << proc << "\tport" << endl;
}

//这里我们结合网络计算器的自定义协议来搞
void AppRead(std::weak_ptr<Connection> conn){
	if(conn.expired()) return;
	auto con = conn.lock();

	cout << "Recv the buffer:" << con->getInbuffer() << endl;
	string res = Calculator().Handler(con->getInbuffer());

	if(res.empty()) return;//不是完整报文

	//接下来是写回
	con->AppendOutBuffer(res);//把结果添加到对应fd的发送缓冲区
	//con->write_cb_(con);//这对吗？ 理论上是没问题 但是这个动作应该是服务器的动作，也就是让服务器发送
	//这就体现出我们的回指指针了
	//con->tcp_ptr_->Writer(con);
	auto tcpptr = con->tcp_ptr_.lock();
	tcpptr->Writer(con);
}

int main(int argc,char* argv[]){
	//std::cout << "Hello CMake" << std::endl;

	if(argc != 2){
		Usage(argv[0]);
		exit(0);
	}

	uint16_t port = stoi(argv[1]);

	//unique_ptr<TcpServer> ps(new TcpServer(port,AppRead)); 
	// std::enable_shared_from_this 提供了 shared_from_this() 方法，
	// 该方法用于在对象内部获取一个指向当前对象的 std::shared_ptr。但这个方法有一个重要的前提条件：
	// 当前对象必须已经被 std::shared_ptr 管理。如果在对象不是通过 std::shared_ptr 
	// 创建和管理的情况下调用 shared_from_this()，会导致未定义行为，通常会抛出 std::bad_weak_ptr异常。
	shared_ptr<TcpServer> ps(new TcpServer(port,AppRead));
	
	ps->Init();
	ps->Loop();

	return 0;
}
