s_lib=libmymath.a
dy_lib=libmyfunc.so

.PHONY:all
all:$(dy_lib) $(s_lib)

$(dy_lib):myprint.o myerror.o
	gcc -shared -o $@ $^
myprint.o:myprint.c
	gcc -fPIC -c $^
myerror.o:myerror.c
	gcc -fPIC -c $^


$(s_lib):mymath.o
	ar -rc $@ $^
mymath.o:mymath.c
	gcc -c  $^

.PHONY:clean
clean:
	rm -rf *.o *.a *.so mylib

.PHONY:output
output:
	mkdir -p mylib/include
	mkdir -p mylib/lib
	cp *.h mylib/include
	cp *.a mylib/lib
	cp *.so mylib/lib
