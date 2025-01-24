#pragma once
#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

//#define Mine 1

const std::string loadSep = " ";//有效载荷的分隔符
const std::string mesSep = "\n";//报文间的分隔符，同时也是报头和有效载荷的分隔符

//"x op y"->"len"\n"x op y"\n
std::string Encode(std::string& content){
    std::string ret = std::to_string(content.size());
    ret += mesSep;
    ret += content; 
    ret += mesSep;

    return ret;
}

//"len"\n"x op y"\n->"x op y"
std::string Decode(std::string& mes){//这个mes可能是一个，半个，多个报文
    auto pos = mes.find(mesSep);
    if(pos == std::string::npos){//没找到，说明长度不知道 直接结束
        return "";
    }

    //cout << "before..." << endl;
    //cout << mes << endl;
    int len = std::stoi(mes.substr(0,pos));
    //cout << "after..." << endl;
    int total = len + std::to_string(len).size() + 2;//对应第一个报文的长度
    if(mes.size() < total){//说明这不是一个完整的报文
        return "";
    }

    std::string ret = mes.substr(pos+1,len);
    
    //erase第一个报文
    mes.erase(0,total);

    return ret;
}


//协议中的自定义类的成员这里就直接搞成public的了
class Request{
public:
    int x;
    int y;
    char op;//+ - *  / %
public:
    std::string serialize(){//struct->string("x op y")
#ifdef Mine//条件编译 两个代码都保存 一个自己写的序列化反序列化 一个Json的
        std::string ret = std::to_string(x);
        ret += loadSep; 
        ret += op;
        ret += loadSep; 
        ret += std::to_string(y);

        return ret;
#else
        Json::Value root;
        root["x"] = x;
        root["op"] = op;
        root["y"] = y;
        //Json::FastWriter w;
        Json::StyledWriter w;
        return w.write(root);
#endif
    }

    bool deserialize(std::string& content){//string("x op y")->struct
#ifdef Mine
        auto pre_pos = content.find(loadSep);
        if(pre_pos == std::string::npos){//说明有效载荷不完整
            return false;
        }

        x = std::stoi(content.substr(0,pre_pos).c_str());
        auto last_pos = content.rfind(loadSep);
        if(last_pos == std::string::npos){//一个完整的有效载荷必须有两个空格
            return false;
        }

        y = std::stoi(content.substr(last_pos + 1).c_str());
        if(pre_pos + 1 != last_pos - 1){//说明op有问题，因为op只能是一个字符
            return false;
        }

        op = content[pre_pos + 1];
        return true;
#else
        Json::Value v;
        Json::Reader r;
        r.parse(content,v);

        x = v["x"].asInt();
        y = v["y"].asInt();
        op = v["op"].asInt();

        return true;
#endif    
    }

    void debugPrint(){
        cout << "构建请求成功：" << x << op << y << endl;
    }
public:
    Request(int data1 = 0,int data2 = 0,char oper = '+'):
    x(data1),y(data2),op(oper){

    }

    ~Request(){}
};

class Response{
public:
    int result;
    int code;//0表示正常 非零表示对应的不正常
public:
    std::string serialize(){
#ifdef Mine
        std::string ret = std::to_string(result);
        ret += loadSep; 
        ret += std::to_string(code);

        return ret; 
#else  
        Json::Value root;
        root["result"] = result;
        root["code"] = code;
        // Json::FastWriter w;
        Json::StyledWriter w;
        return w.write(root);
#endif
    }

    bool deserialize(std::string& content){//"result code"
#ifdef Mine
        auto pos = content.find(loadSep);
        if(pos == std::string::npos) return false;
        
        result = std::stoi(content.substr(0,pos).c_str());
        code = std::stoi(content.substr(pos+1).c_str());
        return true;
#else
        Json::Value v;
        Json::Reader r;
        r.parse(content,v);

        result = v["result"].asInt();
        code = v["code"].asInt();
        return true;
#endif
    }

    void debugPrint(){
        cout << "构建结果成功:result:" << result << ",code:" << code << endl;
    }
public:
    Response(int res = 0,int cod = 0):result(res),code(cod){

    }

    ~Response(){}
};