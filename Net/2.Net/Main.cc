#include <memory>
#include "tcpServer.hpp"

void Usage(const string& proc){
    cout << "Usage:" << endl;
    cout << "\t" << proc << " port[1024+]" << endl; 
}

int main(int argc,char* argv[]){
    if(argc != 2){
        Usage(argv[0]);
        exit(1);
    }
    
    lg.ModifyPrint(ClassFile);

    uint16_t port = stoi(argv[1]);
    unique_ptr<TcpServer> svr(new TcpServer(port));
    
    svr->Init();
    svr->Run();

    return 0;
}