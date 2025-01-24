#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>
using namespace std;

int main(){
    Json::Value temp;
    temp["name"] = "Henry";
    temp["age"] = 20;

    //万能对象 Value存储键值对 key值只能是字符串
    Json::Value root;
    root["x"] = 100;
    root["y"] = 200;
    root["op"] = '+';
    root["decr"] = "This is a operation for adding";
    root["Json"] = temp;//Json套Json

    //Json::FastWriter w;//效率较高的序列化工具
    Json::StyledWriter w;
    string content = w.write(root);
    cout << content << endl;

    //反序列化
    Json::Value v;
    Json::Reader r;
    r.parse(content,v);//反序列化到v里

    Json::Value revt = v["Json"];
    cout << revt["name"].asString() << "  " << revt["age"].asInt() << endl;

    cout << v["x"].asInt() << endl;
    cout << v["y"].asInt() << endl;
    cout << v["op"].asInt() << endl;//char本身也是int
    cout << v["decr"].asString() << endl;

    return 0;
}