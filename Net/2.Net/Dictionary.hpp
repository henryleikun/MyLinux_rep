#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <cstdlib>
using namespace std;

const string filename = "/home/ubuntu/Myhome/Now_test/2.Net/Dict.txt";

bool SplitString(const string& str,string& part1,string& part2){
    auto pos = str.find(":");
    if(pos == string::npos)
       return false;
    
    part1 = str.substr(0,pos);
    part2 = str.substr(pos+1);//默认长度到最后
    return true;
}

class Dictionary{
private:
   unordered_map<string,string> dict;
public:
   void Init(){
      ifstream in(filename,ios::in);
      if(!in.is_open()){
        cerr << "Open file error" << endl;
        exit(1);
      }
      
      string str;
      while(getline(in,str)){//读到文件末尾返回零 每次读到换行符结束也就是一次读一行
         string part1,part2;
         if(SplitString(str,part1,part2)) dict.insert({part1,part2});
      }

      in.close();
   }

   string Find(const string& english){
      auto pos = dict.find(english);
      if(pos != dict.end()) return pos->second;
      else return "Unvalid";
   }
};