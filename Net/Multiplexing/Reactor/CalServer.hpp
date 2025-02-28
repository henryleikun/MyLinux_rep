#pragma once
#include "Protocol.hpp"

enum{
    DivZero = 1,
    ModZero,
    OtherOp
};

class Calculator{
public:
    Response CalculatorHelper(const Request& req){
        int x = req.x;
        int y = req.y;
        char op = req.op;
        int result = 0;
        int code = 0;

        switch (op){
        case '+':
            result = x + y;
            break;
        case '-':
            result = x - y;
            break;
        case '*':
            result = x * y;
            break;
        case '/':
            if(y == 0){
                code = DivZero;
                break;
            }
            result = x / y;
            break;
        case '%':
            if(y == 0){
                code = ModZero;
                break;
            }
            result = x % y;
            break;
        default:
            code = OtherOp;
            break;
        }

        return Response(result,code);
    }

    //"len"\n"x op y"\n——>"len"\n"result code"
    std::string Handler(std::string& package){//将客户端发送来的报文 计算然后序列化发出去
        std::string content = Decode(package);
        if(content.size() == 0) return "";//报文不完整 不能进行计算

        Request req;
        if(req.deserialize(content) == false){//有效载荷的内容有问题
            return "";
        }

        Response res = CalculatorHelper(req);
        content = res.serialize();
        content = Encode(content);

        return content;
    }
};