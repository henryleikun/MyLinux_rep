#include "commd.hpp"

int main(){
    Init init;
    int shmid = CreatShm();
    int fd = open("myfifo",O_RDONLY);
    if (fd < 0){
        log(Fatal, "error string: %s, error code: %d", strerror(errno), errno);
        exit(OPEN_ERROR);
    }

    char* shmaddr = (char*) shmat(shmid,nullptr,0);//挂接，拿到对应的地址
    //sleep(100);
    struct shmid_ds ipcd;

    while(1){
        char c;
        int n = read(fd,&c,sizeof(c));
        if(n == 0) break;//读完了 那边不会写了
        else if(n < 0) break;
        //上述帮助共享内存进行 同步 那边不写 这边会阻塞

        cout << "Client say:" << shmaddr << endl;//做这个动作你得有对应共享内存的读权限，你要在shmget的时候设置。
        sleep(1);

        shmctl(shmid,IPC_STAT,&ipcd);
        cout << "shm size: " << ipcd.shm_segsz << endl;
        cout << "shm nattch: " << ipcd.shm_nattch << endl;
        printf("shm key: 0x%x\n",  ipcd.shm_perm.__key);
        cout << "shm mode: " << ipcd.shm_perm.mode << endl;
    }

    close(fd);//关闭有名管道文件
    shmdt(shmaddr);//取消挂接

    shmctl(shmid,IPC_RMID,nullptr);//释放共享内存
    return 0;
}