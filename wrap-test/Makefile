.PHONY: all func1_so main clean

all: func1_so main 

func1_so:
	gcc -shared -fPIC func1.c -Wl,--wrap,malloc -o libfunc1.so

main:
	gcc -c -o main.o main.c 
	gcc main.o ./libfunc1.so -Wl,--wrap,malloc -o main.exe
clean:
	rm -f *.o *.exe *.so
