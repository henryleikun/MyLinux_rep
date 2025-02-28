#include <iostream>
#include <string>
#include <cstdint>//uint16_t
#include <time.h>
#include <cassert>
#include "Socket.hpp"
#include "Protocol.hpp"

using std::cout;
using std::endl;
using std::string;

void Usage(const string& proc){
    cout << "Usage:"<<endl;
    cout << "\t" << proc << "\tip" << "\tport" << endl;
}

int main(int argc,char* argv[]){
    if(argc != 3){
        Usage(argv[0]);
        return 0;
    }

    string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);

    Socket sock;
    sock.createSock();
    sock.Connect(serverip,serverport);

    srand(time(nullptr));
    string opers = "+-*/%=&$";
    string inbuffer_stream;//和服务端一样 对接收的信息做同样的处理

    int cnt = 1;
    while(cnt <= 15){
        cout << "第" << cnt << "次发送请求..." << endl;
        int x = rand() % 100;
        usleep(1234);
        int y = rand() % 100 + 1;//不为零现在
        char op = opers[rand() % opers.size()];
        Request req(x,y,op);
        req.debugPrint();

        string package = req.serialize();
        package = Encode(package);

        //cout << cnt << "次发的数据:" << package << endl; 
        write(sock.getSockfd(),package.c_str(),package.size());//这个发就是应用层发给内核 也不一定
        //把package的内容全部发完 在TCP层 也可能不会把内核的内容全部发完  但我们在服务端做了很好的处理
           
        char buffer[128];//负责接收服务器发来的"len"\n"result code"\n报文
        //同理 我们也可能接收到的response是不完整的 也要像服务器那样进行处理 这里简化一下
        int n = read(sock.getSockfd(),buffer,sizeof(buffer));
        if(n > 0){
            buffer[n] = 0;
            inbuffer_stream += buffer;
            string content = Decode(inbuffer_stream);
            assert(content.size() != 0);//如果报文解析出错 直接结束客户端 就不用像服务端那样 继续接收了

            Response res;
            assert(res.deserialize(content));//反序列化
            res.debugPrint();//打印接收到的结果
        }
        else
            break;
        
        cnt++;
        sleep(1);
        
    }


    return 0;
}