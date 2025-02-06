#include <iostream>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <sstream>//字符串流
#include <unordered_map>
#include "Socket.hpp"

const string wwwroot = "./wwwroot";
const string sep = "\r\n";
const string homepage = "index.html";

class HttpServer;

struct ThreadData{
    int sockfd_;
    HttpServer* sv_;//静态成员函数想用非静态的

    ThreadData(int sockfd = -1,HttpServer* sv = nullptr):sockfd_(sockfd),sv_(sv){

    }
};

//这个类其实可以另起一个文件 这里就不搞那么多了 主要是原理
class HttpRequest{
public:
    std::vector<string> head_package;//http报文请求空行之前的内容 一行一行的存储
    string text_;//正文
    string method_;
    string url_;
    string version_;//这三个是对请求行的解析 我们主要是想知道url的信息
    string file_path;//基于url确定客户端要访问的文件路径

    string suffix_;//请求文件的后缀
public:
    void Deserialize(string request){//进行反序列化 主要是初始化head_package和text_
        while(true){
            size_t pos = request.find(sep);
            if(pos == string::npos) break;

            string line = request.substr(0,pos);
            if(line.size() == 0) break;//读到空行
            head_package.push_back(line);
            request.erase(0,pos + sep.size());//这里是传值
        }    

        //做完剩下的就是正文 
        //正规点就是先遍历head_package找Content-Length然后用这个去取正文数据，但这里就直接把后面的全部当成正文了
        //因为我们这里就默认 报文就是完整的
        text_ = request;
    }   

    void Parse(){//主要是初始化method url version-解析请求行 并且初始化file_path suffix
        //解析head_package[0]
        std::stringstream ss(head_package[0]);
        ss >> method_ >> url_ >> version_;//第一次用。。。
        
        file_path = wwwroot;//./wwwroot
        if(url_ == "/" || url_ == "/index.html"){
            file_path += "/";
            file_path += homepage;
        }
        else{//  /a/b/c.html
            file_path += url_;      
        }

        auto pos = url_.rfind(".");
        if(pos != string::npos)//找到了
            suffix_ = url_.substr(pos);
        else
            suffix_ = ".html";//默认是html文件
    }

    void DebugPrint(){//调试打印一下反序列化后的http请求
        for(auto& line : head_package){
            //cout << "====================" << endl;//更清晰
            cout << line << endl;
        }
        cout << "method:" << method_ << endl;
        cout << "url:" << url_ << endl;
        cout << "version:" << version_ << endl;
        cout << "text:" << text_ << endl; 

        cout << "======================================================" << endl;
    }
};


class HttpServer{
private:
    Socket listensock_;
    uint16_t port_;
    std::unordered_map<string,string> content_type;
public:
    HttpServer(uint16_t port):port_(port){
        //初始化几个
        content_type[".html"] = "text/html";
        content_type[".png"] = "image/png";
        content_type[".jpg"] = "image/jpeg";
    }

    void Init(){
        listensock_.createSock();
        listensock_.Bind(port_);
        listensock_.Listen();
        lg(Info,"HttpServer Init Done......\n");
    }

    static string ReadFile(const string& path){
        std::ifstream in(path,std::ios::binary);
        if(!in.is_open()) //return "404";
            return "";

        string ret;
        //求一下文件大小
        in.seekg(0,std::ios_base::end);
        auto length = in.tellg();
        in.seekg(std::ios_base::beg);
        ret.resize(length);

        in.read((char*)ret.c_str(),ret.size());

        // string ret;
        // string line;
        // while(getline(in,line)){
        //     ret += line;
        // }
        
        in.close();
        return ret;
    }

    string suffixToContent_type(const string& suffix){
        //我的conten-type里面只有三个
        if(!content_type.count(suffix))
            return "text/html";//如果是没有的就默认这个格式

        return content_type[suffix];
    }

    void HttpRequestHandler(int sockfd){
        char buffer[10240];
        // int n = read(sockfd,buffer,sizeof(buffer));
        int n = recv(sockfd,buffer,sizeof(buffer),0);//这个也可以用来TCP的接收和read一样 0是flags控制非阻塞
        //注意和UDP里使用的recvfrom做区分

        //这里我们其实不能保证读到的报文就一定是一个完整的请求报文 但我们的重点不在这
        //我们这里就认为buffer里读取到了一个完整的报文
        if(n > 0){
            HttpRequest request;
            request.Deserialize(buffer);
            request.Parse();
            request.DebugPrint();

            string path = request.file_path;
            string content = ReadFile(path);

            string res_line = "HTTP/1.1 200 OK\r\n";
            if(content.size() == 0){//没有找到对应的页面
                res_line = "HTTP/1.1 404 Not Found\r\n";
                string err_path = wwwroot;
                err_path += "/err.html";
                content = ReadFile(err_path);
            }
            //res_line = "HTTP/1.1 302 Found\r\n";

            //cout << buffer << endl;//直接打印
            //string content = "hello world\r\n";//这个换行其实无所谓了
            //string content = "<html><body><h1>hello world</h1></body></html>";
            //string content = ReadFile("./wwwroot/index.html");
            string header = "Content-Length: ";
            header += std::to_string(content.size());
            header += "\r\n";
            //header += "Location: http://www.bilibili.com/\r\n";
            header += "Content-Type: ";
            header += suffixToContent_type(request.suffix_);
            header += "\r\n";
            header += "Set-Cookie: Username=Henry\r\n";
            header += "Set-Cookie: Passwd=123456\r\n";

            string res = res_line;
            string empty_line = "\r\n";
            res += header;
            res += empty_line;
            res += content;

            send(sockfd,res.c_str(),res.size(),0);//这是基于TCP发送的另一个接口 注意和UDP的sendto区分
        }
    }

    static void* ThreadRun(void* args){
        pthread_detach(pthread_self());
        ThreadData* td = static_cast<ThreadData*>(args);
        int sockfd = td->sockfd_;
        td->sv_->HttpRequestHandler(sockfd);
        close(sockfd);
        delete td;
        return nullptr;
    }

    void Run(){
        while(true){
            string ip;
            uint16_t port;
            int sockfd = listensock_.Accept(&ip,&port);
            if(sockfd < 0) continue;

            ThreadData* td = new ThreadData;
            td->sockfd_ = sockfd;
            td->sv_ = this;
            pthread_t pid;   
            pthread_create(&pid,nullptr,ThreadRun,(void*)td);
        }
    }

    void Close(){
        listensock_.Close();
    }
};