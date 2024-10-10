.PHONY:all
all:testcpp pro_replace

testcpp:test.cpp
	g++ -o $@ $^ -std=c++11
pro_replace:pro_replace.c
	gcc -o $@ $^ -std=c99

.PHONY:clean
clean:
	rm -rf pro_replace testcpp
