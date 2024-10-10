#include "comm.hpp"
using namespace std;

int main(){
    int fd = open(FIFO_PATH,O_WRONLY);
    if(fd < 0){
        perror("open");
        exit(OPEN_ERROR);
    }

    while(1){
        string mes;
        cout << "Please send:";
        getline(cin,mes);

        write(fd,mes.c_str(),mes.size());
    }

    return 0;
}