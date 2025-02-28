#pragma once
#include <fcntl.h>
#include <iostream>

void SetNonBlock(int fd){
	int f1 = fcntl(fd,F_GETFL);
	if(f1 < 0){
		perror("fcntl");
		return ;
	}

	fcntl(fd,F_SETFL,f1|O_NONBLOCK);
	std::cout << "set " << fd << " Nonblock" << std::endl;
}
