.PHONY:all
all:udpserver udpclient

udpserver:Main.cpp
	g++ -o $@ $^ -std=c++11
udpclient:udpClient.cpp
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -rf udpclient udpserver