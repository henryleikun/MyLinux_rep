#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <unistd.h>

void setNonBlock(int fd){
	int f1 = fcntl(fd,F_GETFL);
	if(f1 < 0){
		perror("fcntl");
		return ;
	}

	fcntl(fd,F_SETFL,f1|O_NONBLOCK);
	std::cout << "set " << fd << " Nonblock" << std::endl;
}

int main(){
	setNonBlock(0);
	sleep(1);

	while(true){
		char buffer[128];
		int n = read(0,buffer,sizeof(buffer) - 1);
		if(n > 0){
			buffer[n - 1] = 0;
			std::cout << "echo:" << buffer << std::endl;
		}
		else if( n == 0){
			break;//read done
		}
		else{
			if(errno == EWOULDBLOCK){//非阻塞
				std::cout << "read not ready" << std::endl;
				sleep(1);
				//做其他事
			}
			else{
				//错误
				break;
			}
			//其实还可能信号中断 中断也会返回<0
		}
	}

	return 0;
}