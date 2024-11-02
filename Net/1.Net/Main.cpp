#include <memory>
#include "udpServer.hpp"

void Usage(const string& proc){//用法
    cout << "\n\rUsage: " << proc << " port[1024+]\n" << endl;
}

string Handle(string& str){
    string ret = "Server has handled the string:";
    ret += str;
    return ret;
}

int main(int argc,char* argv[]){
    if(argc != 2){
        Usage(argv[0]);
        exit(0);
    }

    uint16_t port = stoi(argv[1]);
    unique_ptr<UdpServer> server(new UdpServer(port));
    server->Init();

    //对从Client收到的信息做Handle处理
    server->Run(Handle);

    return 0;
}
