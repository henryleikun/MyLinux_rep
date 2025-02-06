#include <iostream>
#include <memory>//unique_ptr
#include "HttpServer.hpp"

void Usage(const string& proc){
    cout << "Usage" << endl;
    cout << "\t" << proc << "\tport[1024+]" << endl;
}

int main(int argc,char* argv[]){
    if(argc != 2){
        Usage(argv[0]);
        return 0;
    }

    uint16_t port = std::stoi(argv[1]);
    std::unique_ptr<HttpServer> psv(new HttpServer(port));
    psv->Init();
    psv->Run();

    return 0;
}