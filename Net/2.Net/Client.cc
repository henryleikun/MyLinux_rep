#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdint>//uint16_t
#include "log.hpp"
using namespace std;

Log lg;
void Usage(const string& proc){
    cout << "Usage:" << endl;
    cout << "\t" << proc << " port[1024+]" << " ip"<<endl; 
}

int main(int argc,char* argv[]){
    if(argc != 3){
        Usage(argv[0]);
        exit(0);
    }
    
    //确定目的ip和端口
    uint16_t port = stoi(argv[1]);
    string ip = argv[2];

    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_aton(ip.c_str(),&(server.sin_addr));

   //  //创建套接字
   // int sockfd = socket(AF_INET,SOCK_STREAM,0);
   // if(sockfd < 0){  
   //    lg(Fatal,"Socket create error,sockfd:%d,error str:%s",sockfd,strerror(errno));
   //    exit(1);
   // }

   // //不需要自己绑定
   // int n = connect(sockfd,(sockaddr*)&server,sizeof(server));
   // if(n < 0){
   //    lg(Fatal,"Connet error,errno:%d,errstr:%s",errno,strerror(errno));
   //    exit(2);
   // }
    
   while(1){//该循环的每一次都意味着一次新的服务 都要重新建立连接 
      int cnt = 5;//重连次数
      bool isreconnect = false;//默认不用重连
      int sockfd = -1;

      do{
         //创建套接字
         sockfd = socket(AF_INET,SOCK_STREAM,0);
         if(sockfd < 0){  
            lg(Fatal,"Socket create error,sockfd:%d,error str:%s",sockfd,strerror(errno));
            exit(1);
         }

         //不需要自己绑定
         int n = connect(sockfd,(sockaddr*)&server,sizeof(server));
         if(n < 0){//连接失败
            close(sockfd);//你连失败了 但是你创建了套接字 你是要重连 但是你得把之前的关掉
            isreconnect = true;
            cnt--;
            cout << "Connect error... reconnect " << cnt << endl;
            sleep(2);
         }
         else
            break;//连上了就退出 要不然就会一直创建 一直连 一直成功...
      }while(cnt && isreconnect);

      if(cnt == 0){//真连不上了
         cout << "User offline..." << endl;
         break;
      }
   
       //sleep(5);
       cout << "Please Enter# ";
       string message;
       getline(cin,message);
       int wn = write(sockfd,message.c_str(),message.size());
       //cout << "依旧能发送..." << endl;
       if(wn < 0){//写入错误
         //lg(Fatal,"Write Error,process will close the link...");
         //break; 不用break了 就走到下面然后close掉进行下一次的重连就行 
         lg(Fatal,"Write Error,trying to apply for next service..."); //重新打印个日志
       }
       else if(wn == 0){//处理getline的输入空行的情况
          lg(Warning,"Don't send a empty data.");
          close(sockfd);//其实如果输入空行不用重新建立连接 应该回到Please Enter的逻辑处 但我不想用goto
          continue;
       }

       char recvbuffer[4096];
       //cout << "Before reciving..." << endl;
       int rn = read(sockfd,recvbuffer,sizeof(recvbuffer));
       //cout << "After reciving..." << endl;
       if(rn > 0){//读取成功
          recvbuffer[rn] = 0;
          cout << recvbuffer << endl;
          //break; 不用break进行下一次服务
       }
       else if(rn == 0){//服务端关闭
          //lg(Info,"Sever has closed connection,this app will closed soon...");
          lg(Info,"Sever has closed connection,trying to reconnect...");
          //break;
       }
       else{//读端异常
          //lg(Warning,"App read the server error,closing app...");
          //break;
          lg(Warning,"App read the server error,trying to apply for next service...");
       }

       close(sockfd);
    }
    
    
    return 0;
}