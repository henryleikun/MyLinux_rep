#include "TcpServer.hpp"
#include "CalServer.hpp"
#include "Daemon.hpp"
//#include <unistd.h>

void Usage(const string& proc){
    cout << "Usage:"<<endl;
    cout << "\t" << proc << "\tport[1024+]" << endl;
}

int main(int argc,char* argv[]){
    if(argc != 2){
        Usage(argv[0]);
        return 0;
    }

    uint16_t port = std::stoi(argv[1]);
    ServerCal scal;
    TcpServer* ts = new TcpServer(port,std::bind(&ServerCal::Calculator,scal,std::placeholders::_1));
    ts->InitServer();
    //Daemon();
    //daemon(0,0);
    ts->Run();


    //测试自定义协议和序列化和反序列化
    // struct Request req(100,200,'+');
    // //序列化
    // string content = req.serialize();
    // cout << content << endl;
    // content = Encode(content);
    // cout << content;//完整报文

    // //反序列化
    // struct Request r;
    // content = Decode(content);
    // r.deserialize(content);
    // cout << r.x <<endl;
    // cout << r.op << endl;
    // cout << r.y <<endl;

    return 0;
}