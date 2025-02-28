#include <memory>
#include "SelectServer.hpp"

void Usage(const string& proc){
    cout << "Usage:"<<endl;
    cout << "\t" << proc << "\tport" << endl;
}

int main(int argc,char* argv[]){
	if(argc != 2){
		Usage(argv[0]);
		exit(0);
	}

	uint16_t port = stoi(argv[1]);
	unique_ptr<SelectServer> ps(new SelectServer(port));
	ps->Init();
	ps->Run();

	return 0;
}