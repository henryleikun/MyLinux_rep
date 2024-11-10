#include <memory>
#include <cstdio>
#include <vector>
#include "udpServer.hpp"

void Usage(const string& proc){//用法
    cout << "\n\rUsage: " << proc << " port[1024+]\n" << endl;
}

string Handle(const string& str){
    cout << str << endl;//先把client发来的信息打印在服务器上
    string ret = "Server has handled the string:";
    ret += str;
    return ret;
}

string Handle_mes(const string& str,uint16_t port,const string& ip){
    cout << "[" << ip << ":" << port <<"]# " << str << endl; 
    string ret = "Server has handled the string:";
    ret += str;
    return ret;
}

//检查命令是否违法
bool CheckSafe(const string& cmd){
   vector<string> unsafe = { "rm","mv","cp","unlink","kill","uninstall","apt","top" };

   for(auto& e : unsafe){
     auto pos = cmd.find(e);
     if(pos != string::npos)//如果找到 说明不安全
        return false;
   } 

   return true;//安全
}

string Excecutecmd(const string cmd){
    cout << "client sends the command:" << cmd << endl;
    if(!CheckSafe(cmd)) return "Bad man";

    //安全后
    FILE* fp = popen(cmd.c_str(),"r");
    if(fp == nullptr){
        perror("popen\n");
        return "Error";
    }
    
    string ret;
    char buffer[4096];
    while(1){
        //fgets读到\n结束会返回对应的字符串指针，读到文件末尾EOF会返回NULL 或者读到n-1个字符为止(给\0空出)
        //因为fp可能有多行数据，所以读取的最终目的是读到文件结尾
        char* get = fgets(buffer,sizeof(buffer),fp);
        if(get == nullptr) break;
        ret += buffer;
    }

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
    //server->Run(Handle);
    //server->Run(Handle_mes);
    server->Run();

    return 0;
}
