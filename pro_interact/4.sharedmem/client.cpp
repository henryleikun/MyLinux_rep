#include "commd.hpp"

int main(){
    int shmid = GetShm();
    int fd = open("myfifo",O_WRONLY);
    if (fd < 0){
        log(Fatal, "error string: %s, error code: %d", strerror(errno), errno);
        exit(OPEN_ERROR);
    }

    char* shmaddr = (char*)shmat(shmid,nullptr,0);//挂接
    while(1){
        cout << "Please enter:";
        fgets(shmaddr,SSIZE,stdin);

        //write(fd,"c",sizeof(char));
    }

    close(fd);
    shmdt(shmaddr);
    return 0;
}