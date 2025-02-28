//#include <iostream>
#include "EpollServer.hpp"

void Usage(const string& proc){
    cout << "Usage:"<<endl;
    cout << "\t" << proc << "\tport" << endl;
}


int main(int argc,char* argv[]){
	//std::cout << "Hello CMake" << std::endl;

	if(argc != 2){
		Usage(argv[0]);
		exit(0);
	}

	unique_ptr<EpollServer> ps(new EpollServer());
	ps->Init();
	ps->Run();
	
	return 0;
}
