#include <memory>
#include "PollServer.hpp"

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
	unique_ptr<PollServer> ps(new PollServer(port));
	ps->Init();
	ps->Run();

	return 0;
}