#include "comm.hpp"
#include "log.hpp"

using namespace std;

int main(){
    //cout << sum(3,1,2,3) << endl;

    //CreatPipe();//创建有名管道
    Init init;
    Log log;
    log.ModifyPrint(3);
    int fd = open(FIFO_PATH,O_RDONLY);

    if(fd < 0){
        //perror("open");
        log(Fatal,"string error:%s,error code:%d\n",strerror(errno),errno);
        exit(OPEN_ERROR);
    }

    log(Info,"Open file done,string error:%s,error code:%d\n",strerror(errno),errno);


    while(1){
        char buffer[SIZE];

        int n = read(fd,buffer,sizeof(buffer));
        if(n > 0){
            buffer[n] = 0;//当字符串来处理
            cout << "client say:" << buffer << endl;
        }
        else if(n == 0){
            //printf("Read file done\n");  
            log(Info,"Read file done,string error:%s,error code:%d\n",strerror(errno),errno);
            break;
        }
        else break;
    }

    //ClosePipe();

    return 0;
}