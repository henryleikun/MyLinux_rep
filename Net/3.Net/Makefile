.PHONY:all
all:calclient calserver

Flag=#-DMine=1
Lib=-ljsoncpp

calclient:CalClient.cpp
	g++ -o $@ $^ -std=c++11 $(Lib) $(Flag)
calserver:CalServer.cpp
	g++ -o $@ $^ -std=c++11 $(Lib) $(Flag)

.PHONY:clean
clean:
	rm -rf calclient calserver