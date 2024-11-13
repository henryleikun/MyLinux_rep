.PHONY:all
all:tcpserverd tcpclient

tcpserverd:Main.cc
	g++ -o $@ $^ -std=c++11 -lpthread
tcpclient:Client.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -rf tcpclient tcpserver