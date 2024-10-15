#pragma once
#include <iostream>
#include <string>

std::string opers = "+-*/%";
enum {
    DivZero = 1,
    ModZero,
    OpError
};

class Task{
private:
   int data1_;
   int data2_;
   char op_;

   int result_;
   int exitcode_;//判断结果是否正确 0是正确的
public:
   Task() = default;

   Task(int data1,int data2,char op):data1_(data1),data2_(data2),op_(op),result_(0),exitcode_(0){
   
   }

   void Run(){
      switch(op_){
         case '+' : 
            result_ = data1_ + data2_;
            break;
        case '-' : 
            result_ = data1_ - data2_;
            break;
        case '*' : 
            result_ = data1_ * data2_;
            break;
        case '/' : {
            if(data2_ == 0) exitcode_ = DivZero;
            else result_ = data1_ / data2_;
            break;
        }
        case '%' : {
            if(data2_ == 0) exitcode_ = ModZero;
            else result_ = data1_ / data2_;
            break;
        }
        default: exitcode_ = OpError; break;
      }
   }

   std::string GetResult(){
      std::string ret = std::to_string(data1_);
      ret += " + ";
      ret += std::to_string(data2_);
      ret += " = ";
      ret += std::to_string(result_);
      ret += "[code:";
      ret += std::to_string(exitcode_);
      ret += "]";
      return ret;
   }

   std::string GetTask(){//便于打印相关日志
      std::string ret = std::to_string(data1_);
      ret += " + ";
      ret += std::to_string(data2_);
      ret += " = ";
      ret += "?";
      return ret;
   }
};