#include <iostream>
using namespace std;

int main(int argc,char* argv[],char* env[]){
  //cout << "Hello C++ Linux\n" << endl;
  cout << argv[0] << "的命令行参数有：" << endl;
  for(int i = 1; argv[i]; i++){
   cout << "[" << i << "]:" << argv[i] << endl;
  }
  cout << "环境变量有：" << endl ;
  for(int i = 0; env[i]; i++){
   cout << "[" << i << "]:" << env[i] << endl;
  }
  return 0;
}
