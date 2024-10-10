#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <sys/wait.h>
#include <functional>
#include "Task.hpp"
using namespace std;

const int N = 10;
vector<function<void()>> tasks{task1,task2,task3,task4,task5};

//对管道文件做描述
class Pipe{
public:
   int _fd;//父进程对应的写端
   pid_t _propid;//子进程的pid
   string _name;//子进程name
   
   Pipe(int fd,pid_t propid,string name):_fd(fd),_propid(propid),_name(name){

   }
};

void Run_childpro(){
   int cmdcode;
   while(1){
     int n = read(0,&cmdcode,sizeof(cmdcode));

     //cout << "child " << getpid() << " get a taskcode: " << cmdcode << endl << endl;
     if(n > 0){
      cout << "child " << getpid() << "#:";
      tasks[cmdcode](); 

      cout << endl;
     }
     else
       break;
   }
}

void Menu(){
     cout << "Please choose your task code:" << endl;

    cout << "###################################" << endl;
    cout << "#  1. 检测系统更新 2. 更新用户信息  #" << endl; 
    cout << "#  3. 匹配对局     4. 更新状态     #" << endl;
    cout << "#  5. 刷新野区     0. 退出         #" << endl;
    cout << "###################################" << endl;
}

//这里有个算是标准吧 输出型参数：*  输入型参数：& 输入输出型：&
void Init(vector<Pipe>* pipes){
    vector<int> fd_del_tab;

    //创建子进程
    for(int i = 0; i < N; i++){
       int pipefd[2]{};
       int n = pipe(pipefd);//创建对应管道

       //if(n < 0) return 1;
       
       pid_t id = fork();//创建子进程
       //if(id < 0) return 1;
       if(id == 0){//child
         close(pipefd[1]);//子进程关闭对应写端
         dup2(pipefd[0],0);//子进程本来是从0读的，这里让管道的读端代替对应的0 下面的Run函数就不用有参数了
         //cout << "child fd :" << pipefd[0] << endl;

         for(auto& e: fd_del_tab) close(e);//关闭对应父进程继承下来的写端fd

         Run_childpro();
         exit(0);
       }
      
       //father
       close(pipefd[0]);//父进程关闭读端
       string name = "process" + to_string(i);
       pipes->push_back(Pipe(pipefd[1],id,name));
      
       fd_del_tab.push_back(pipefd[1]);//保存父进程的写端fd
    }
}

void Allocate_task_1(const vector<Pipe>& pipes){
     srand(time(nullptr)); 
     for(int i = 0; i < 50; i++){//先以50个任务为例 先随机
      //选择进程
      int pronum = rand() % pipes.size();
      
      //发送任务码
      //int cmdcode = rand() % 20;
      int cmdcode = rand() % tasks.size();
      cout << "father send taskcode " << cmdcode << " to " << pipes[pronum]._propid << "——" << pipes[pronum]._name << endl;
      write(pipes[pronum]._fd,&cmdcode,sizeof(cmdcode));

      sleep(1);//每隔一秒发送一个
    }
}

void Allocate_task_2(const vector<Pipe>& pipes){//轮询
     int which = 0;
     for(int i = 0; i < 50; i++){
      //发送任务码
      //int cmdcode = rand() % 20;
      int cmdcode = rand() % tasks.size();
      cout << "father send taskcode " << cmdcode << " to " << pipes[which]._propid << "——" << pipes[which]._name << endl;
      write(pipes[which]._fd,&cmdcode,sizeof(cmdcode));
      which++;
      which = which % pipes.size();

      sleep(1);//每隔一秒发送一个
    }
}

void Allocate_task_3(const vector<Pipe>& pipes){//手动
     int select = 0;
     int which = 0;

     while(1){
      Menu();
      cin >> select;
      if(select > 0 && select < 6){
        int cmdcode = select - 1;
        cout << "father send taskcode " << cmdcode << " to " << pipes[which]._propid << "——" << pipes[which]._name << endl;
        write(pipes[which]._fd,&cmdcode,sizeof(cmdcode));
        which++;
        which = which % pipes.size();
      }

      if(select == 0)
        break;//0退出，但除了0之外的错误重新输入
    }
}


void Recycle(const vector<Pipe>& pipes){
    for(int i = 0; i < pipes.size(); i++){
       close(pipes[i]._fd);
       int rid = waitpid(pipes[i]._propid,nullptr,0);
       if(rid < 0){
       perror("等待失败\n");
       }
    }

//    for(int i = pipes.size() - 1; i >= 0; i--){
//        close(pipes[i]._fd);
//        int rid = waitpid(pipes[i]._propid,nullptr,0);
//        if(rid < 0){
//        perror("等待失败\n");
//        }
//     }

//    for(int i = 0; i < pipes.size(); i++){
//      int rid = waitpid(pipes[i]._propid,nullptr,0);//等的是pid不是文件描述符别晕
//      //cout << "等待pid:" << rid << endl;
//      if(rid < 0){
//        perror("等待失败\n");
//        //return 1;
//      }
//    }
} 


int main(){
    vector<Pipe> pipes;//用来组织管道的vector
   
    //初始化对应的组织
    Init(&pipes);
    // for(auto& e : pipes){
    //     cout << "father fd: " << e._fd << endl; 
    // }
   
    //组织好了对应的管道和子进程，下面进行控制进程
    //分派任务要选择进程，这里给出两种实现，1. 随机选择 2. 轮询
    Allocate_task_3(pipes);//随机
    
    //回收子进程
    Recycle(pipes); 

    return 0;
}