test:test.o main.o
	gcc -o test test.o main.o
test.o:test.c
	gcc -c test.c -o test.o
main.o:main.c
	gcc -c main.c -o main.o

.PHONY:clean
clean:
	rm -rf *.o test
